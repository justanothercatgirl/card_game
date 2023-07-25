#include <algorithm>
#include <iostream>
#include <iomanip>
#include <utility>
#include <random>
#include <array>
#include <queue>
#include <list>

enum type { m1, m2, m3, m4 };
enum cost { c2, c3, c4, c6, c7, c8, c9, c10, c11 };
    
struct card
{
    type t;
    cost c;
};

enum struct choice_flags
{
    player_1_card = 0x01;
    player_2_card = 0x02;
    deck_1_card   = 0x04;
}

void print(card c)
{
    std::cout << "type: " << c.t  << ", cost : " << c.c << '\n';
}

std::queue<card> generate_queue()
{
    std::array<card, 36> tmp;
    int index = 0;
    
    const std::array<type, 4> types {m1, m2, m3, m4};
    const std::array<cost, 9> costs {c2, c3, c4, c6, c7, c8, c9, c10, c11};
    for(type a: types)
    for(cost b: costs)
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
	std::cout << "player: \n";
	for(auto x: arr)
		print(x);
		
	std::cout.put('\n');
}

template <size_t size>
void do_auto_exchanges(
	std::array<card, size> &p_dominant,
	std::array<card, size> &p_submissive,
	std::queue<card> &q)
{
	//do two times
	for(char _ = 0; _ < 2; ++_)
	{
		auto x1 = find_biggest(p1);
		auto x2 = p2.begin() + std::distance(p1.begin(), x1);
		std::iter_swap(x1, x2);
	}
	
	auto x1 = find_biggest(p1);
	q.push(*x1);
	*x1 = q.front();
	q.pop();
}

template <size_t size>
char get_response_flags()
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
    char flags = 0;
    for(char x : answer)
    {
        switch(x)
        {
        case '1': flags |= choice_flags::player_1_card; break;
        case '2': flags |= choice_flags::player_2_card; break;
        case '3': flags |= choice_flags::deck_1_card; break;
        case ' ': [[fallthrough]]
        case ',': [[fallthrough]]
        case '\t': break;
        default: std::cin << "invalid option: " << std::quoted(x) << '\n'; goto input;
        }
    }
    return flags
}

template <size_t size>
void parse_flagged_input(
    char flags, 
    std::array<char, size>& p_dominant, 
    std::array<char, size>& p_submissive,
    std::queue<char>& q)
{
    char index;
    
    if (flags & choice_flags::player_1_card)
    {
exchange:
        std::cout << "what card would you like to exchange?\n";
        std::cin >> index;
        std::iter_swap(
            p_dominant.begin() + index - 1,
            p.submissive.begin() + index - 1
            );
            
        std::cout << "your new card set:\n";
        print_player(player_dominant);
    }
    if (flags & choice_flags::player_2_card)
    {
        flags &= ~choice_flags::player_2_card;
        goto exchange;
    }
    if (flags & choice_flags::deck_1_card)
    {
        std::cout << "what card would you like to exchange?\n";
        std::cin >> index;
        q.push(p_dominant.begin() + index - 1);
    }
}

int main()
{
	std::queue<card> q(generate_queue());
	auto players[2] = {give_cards<5>(q), give_cards<5>(q)};
	
	for(int turn{0}; ; turn = (turn+1)%2)
	{
		
	}
	
	[[unreachable]]
    return -1;
}
