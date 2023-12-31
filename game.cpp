//#define DEBUG_MODE
//#define INTERACTIVE_MODE

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <utility>
#include <random>
#include <thread>
#include <chrono>
#include <array>
#include <queue>
#include <list>
#include <map>

#ifdef DEBUG_MODE
	#define __DEBUG__(string) std::cout << "\e[91m" << string << "\e[0m\n"
	#include <bitset>
#else
	#define __DEBUG__(string) ;
#endif

#ifdef INTERACTIVE_MODE
	#define INTCOUT(string) std::cout << string
#else
	#define INTCOUT(string)
#endif
	

enum type { m1 = 1, m2, m3, m4 };
enum cost { c2 = 2, c3, c4, c6 = 6, c7, c8, c9, c10, c11 };
const std::array<type, 4> types = {m1, m2, m3, m4};
const std::array<cost, 9> costs = {c2, c3, c4, c6, c7, c8, c9, c10, c11};

const std::array<type, 5> __types__ = {m1, m1, m2, m3, m4};
const std::array<cost, 12> __costs__ = {c2, c2, c2, c3, c4, c4, c6, c7, c8, c9, c10, c11};


struct card
{
	type t;
	cost c;
	
#ifdef DEBUG_MODE
	friend std::ostream& operator<<(std::ostream& out, const card& c);
#endif

};

#ifdef DEBUG_MODE
std::ostream& operator<<(std::ostream& out, const card& c)
{
	out << "type: " << c.t << ", cost: " << c.c;
	return out;
}
#endif

enum /*class*/ choice_flags
{
	player_1_card = 0x01,
	player_2_card = 0x02,
	deck_1_card   = 0x04,
};

void print(card c)
{
#ifdef INTERACTIVE_MODE
	std::cout << "type: " << c.t  << ", cost : " << c.c << '\n';
#else
	std::cout << c.t << ' ' << c.c << '\n';
#endif
}

std::queue<card> generate_queue()
{
	std::array<card, 36> tmp;
	int index = 0;
	
	for(const type a: types)
	for(const cost b: costs)
	{
		tmp[index] = card{a, b};
		index++;
	}
	
	std::random_device rd;
	std::mt19937 gen(rd());
	std::shuffle(tmp.begin(), tmp.end(), gen);
	
	std::queue<card> ret;
	for(const auto& x: tmp)
		ret.push(x);
	
	return ret;
}

template <size_t size>
inline std::array<card, size> 
give_cards(std::queue<card>& q)
{
	std::array<card, size> ret;
	for(auto &x: ret)
	{
		x = q.front();
		q.pop();
	}
	return ret;
}

template <size_t size> inline 
typename std::array<card, size>::iterator
find_biggest(std::array<card, size> &arr)
{
	auto ret = arr.end()-1;
	for(auto it = arr.begin(); it != arr.end(); ++it)
		if((*it).c > (*ret).c) ret = it;
		
	return ret;
}

template <size_t size> void 
print_player(std::array<card, size> arr)
{
	for(auto x: arr)
		print(x);
#ifdef INTERACTIVE_MODE
	std::cout.put('\n');
#endif
}

template <size_t size>
void do_auto_exchanges(
	std::array<card, size> &p_dominant,
	std::array<card, size> &p_submissive,
	std::queue<card> &q)
{
	//TODO: decide whether exchamge is needed
	//do two times
	for(char _ = 0; _ < 2; ++_)
	{
		auto x1 = find_biggest(p_dominant);
		auto x2 = p_submissive.begin() + std::distance(p_dominant.begin(), x1);
		std::iter_swap(x1, x2);
	}
	
	auto x1 = find_biggest(p_dominant);
	q.push(*x1);
	*x1 = q.front();
	q.pop();
}

template <size_t size>
unsigned int get_response_flags(const std::array<card, size>& arr)
{
	std::cout << "your cards are:\n";
	print_player(arr);
	
input:
	
	std::cout << "would you like to\n"
		"1. exchange 1 card with opponent?\n"
		"2. exchange 2 cards with opponent?\n"
		"3. exchange card with deck?\n"
		"<e.g.: answer \"13\" will make you exchange 1 card with opponent and 1 with deck>\n";
		
	std::string answer;
	std::cin >> answer;
	unsigned int flags = 0;
	for(char x : answer)
	{
		switch(x)
		{
		case '1': flags |= choice_flags::player_1_card; break;
		case '2': flags |= (choice_flags::player_2_card | choice_flags::player_1_card); break;
		case '3': flags |= choice_flags::deck_1_card; break;
		case ' ': 
		case ',': 
		case '\t': break;
		default: std::cout << "invalid option: '" << x << "'\n"; goto input;
		}
	}
	__DEBUG__(std::bitset<8>(flags));
	return flags;
}

template <size_t size>
void parse_flagged_input(
	unsigned int flags, 
	std::array<card, size>& p_dominant, 
	std::array<card, size>& p_submissive,
	std::queue<card>& q)
{
	
	int index;  //I DECLARED IT CHAR AND IT LOST ME LIKE FUCKING 1.5 HOURS HOLY SHIT MAN
	
	if (flags & choice_flags::player_1_card)
	{
exchange:
		std::cout << "what card would you like to exchange?\n";
		std::cin >> index;
		
		if(index < 1 || index > size)
		{
			std::cout << "please enter valid index (1-5)\n";
			goto exchange;
		}
		
#ifdef DEBUG_MODE
		print_player(p_submissive);
		std::cout << index-1 << '\n';
		std::cout << *p_dominant.begin() << '\n';
		std::cout << *p_submissive.begin() << '\n';
		__DEBUG__("before exchange: pl.1: " << *(p_dominant.begin() + index - 1) 
			<< ", pl.2: " << *(p_submissive.begin() + index - 1));
#endif
		std::iter_swap(
			p_dominant.begin() + index - 1,
			p_submissive.begin() + index - 1
			);
		
#ifdef DEBUG_MODE
		print_player(p_submissive);
		std::cout << index-1 << '\n';
		std::cout << *p_dominant.begin() << '\n';
		std::cout << *p_submissive.begin() << '\n';
		__DEBUG__("after exchange: pl.1: " << *(p_dominant.begin() + index - 1) 
			<< ", pl.2: " << *(p_submissive.begin() + index - 1));
#endif

		std::cout << "your new card set:\n";
		print_player(p_dominant);
		__DEBUG__("player_1_card_flag reached");
	}
	if (flags & choice_flags::player_2_card)
	{
		flags &= ~choice_flags::player_2_card;
		__DEBUG__("player_2_card_flag reached");
		goto exchange;
	}
	if (flags & choice_flags::deck_1_card)
	{
		std::cout << "what card would you like to exchange?\n";
		std::cin >> index;
		
		q.push(*(p_dominant.begin() + index - 1));
		*(p_dominant.begin() + index - 1) = q.front();
		q.pop();
		
		std::cout << "your new card set:\n";
		print_player(p_dominant);
		
		__DEBUG__("deck_1_card_flag reached");
	}
}

inline void 
win_statistics(const std::map<std::string, int>& m)
{
	std::cout << "\e[36mwins:\n";
	for(const auto& [player, wins] : m)
		std::cout << std::setw(10) << std::left 
				  << player << " : " << wins << std::endl;
	std::cout << "\e[0m";
}

template <size_t size> int 
count(const std::array<card, size>& player)
{
	int ret = 0;
	for(const auto& [_, value] : player)
		ret += value;
	return ret;
}

std::string game()
{
	using namespace std::chrono_literals;
	
	std::queue<card> q(generate_queue());
	std::array<card, 5> player1 = give_cards<5>(q), 
						player2 = give_cards<5>(q);
	
	for(bool turn{true}; ; turn = !turn)
	{
		if (turn)
		{
			__DEBUG__("turn player");
			unsigned int flags = get_response_flags(player1);
			parse_flagged_input(flags, player1, player2, q);
		}
		else
		{
			__DEBUG__("turn computer");
			std::cout << "player 2 doing things...";
			std::this_thread::sleep_for(2s);
			do_auto_exchanges(player2, player1, q);
			std::cout << "\nplayer 2 done!\nyour cards are:\n";
			
			print_player(player1);
			std::cout << "go for 1 more round? [Y/n]\n";
			char input;
			std::cin >> input;
			switch(input)
			{
			case 'n': 
			case 'N': goto end;
			default : ;
			}
		}
	}

end:
	std::cout << "your cards are: \n"; print_player(player1);
	std::cout <<"Bot's cards are: \n"; print_player(player2);
	return count(player1) <= count(player2) ? "Player 1" : "Bot";

}

int interactive()
{
	std::cout << "\e[1;4;33mthe goal of this game is to get cards that will\n"
				 "sum up to the minimal amount possible. every turn you\n"
				 "can exchange cards with other player or with deck.\n"
				 "exchange with player is always done before the exchange\n"
				 "with deck. after each turn you can either finish the game\n"
				 "or go for 1 more round. after each game, the win statistics\n"
				 "are printed.\n\e[0m";
	
	std::map<std::string, int> wins = {{"Player 1", 0}, {"Bot", 0}};
	
	bool _continue = true;
	do {
		wins[game()]++;
		win_statistics(wins);
		std::cout << "continue? [Yn]\n";
		char input;
		std::cin >> input;
		switch(input)
		{
		case 'n':
		case 'N':
			_continue = false;
		default:
			;
		}

	} while(_continue);
	
	return 0;
}

template <size_t size>
int input_player(std::array<card, size> &pl, char** inputs)
{
	int i = 0;
	for(auto& elem: pl)
	{
		elem.t = __types__[std::stoi(inputs[i])]; i++;
		elem.c = __costs__[std::stoi(inputs[i])]; i++;
	}
	return i;
}

int input_queue(std::queue<card>& q, char** inputs)
{
	if (inputs[0][1] != 'q') std::terminate(); //unneeded
	int size = std::stoi(inputs[0]+2);
	for(int i = 1; i <= 2*size; i+=2)
	{
		card elem;
		elem.t = __types__[std::stoi(inputs[i])]; i++;
		elem.c = __costs__[std::stoi(inputs[i])]; i++;
		q.push(elem);
	}
	return size;
} 

template <size_t size>
void print_generated_info(
	const std::array<card, size> &p1, 
	const std::array<card, size> &p2,
	std::queue<card> q)
{
	std::cout << "player1:\n";
	print_player(p1);
	std::cout << "player2:\n";
	print_player(p2);
	std::cout << "deck:\n";
	while(!q.empty())
	{
		card elem = q.front();
		std::cout << elem.t << ' ' << elem.c << '\n';
		q.pop();
	}
}

int CLI(int argc, char* argv[])
{
	std::array<std::array<card, 5>, 2> players = {};
	std::queue<card> q;
	for(int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		switch (argv[i][1])
		{
		case 'p': 
		{
			if (argv[i][2] != '1' && argv[i][2] != '2') std::terminate();
			i += input_player(players[argv[i][2]-'0'-1], argv+i+1);
			break;
		}
		case 'q':
		{
			i += input_queue(q, argv+i);
			break;
		}
		case 'g':
		{	
			std::queue<card> q(generate_queue());
			for(auto &a: players) a = give_cards<5>(q);
			print_generated_info(players[0], players[1], q);
			return 0;
		}
		case 'i':
		{
			print_generated_info(players[0], players[1], q);
			break;
		}
		
		default: std::terminate();
		
		}
	}
	return 0;
}

int main(int argc, char** argv)
{
#ifdef INTERACTIVE_MODE
	return interactive();
#else
	//const char* arrr[] = {"a.out", "-p2", "1", "6", "2", "5", "3", "1", "4", "10", "1", "11"};
	//const char* arrr[] = {"a.out", "-q3", ""};
	return CLI(/*sizeof(arrr)/sizeof(char*), (char**)arrr*/ argc, argv);
#endif
}