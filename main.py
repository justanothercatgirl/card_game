import typing
import sys, subprocess, os
from PyQt5.QtWidgets import QApplication, QPushButton, QWidget, QGridLayout, QHBoxLayout, QMessageBox


class Window(QWidget):
    b1: QPushButton
    b2: QPushButton
    layout: QGridLayout
    button_layout: QHBoxLayout

    glob_test = 'lol'

    def pressed1(self):
        QMessageBox(parent=None, text="hello").exec()

    def pressed2(self):
        QMessageBox(parent=None, text=self.glob_test).exec()

    def call_cpp(self, args: str):
        game_iteration = subprocess.Popen(args.split(), shell=True, stdout=subprocess.PIPE)
        return game_iteration.communicate()[0].decode('utf-8')

    def __init__(self):
        super(Window, self).__init__()
        super(Window, self).setGeometry(50, 50, 1000, 699)
        self.global_test = self.call_cpp("test.exe a b cd ef")
        print(self.global_test)
        b1, b2 = QPushButton("button 1"), QPushButton("Button 2")
        layout = QGridLayout(self)
        button_layout = QHBoxLayout()
        layout.addLayout(button_layout, 1, 1)
        button_layout.addWidget(b1)
        button_layout.addWidget(b2)
        b1.clicked.connect(self.pressed2)
        b2.clicked.connect(self.pressed1)


def main():
    a = QApplication(sys.argv)
    w = Window()
    w.show()
    return a.exec()


if __name__ == '__main__':
    main()

