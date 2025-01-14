//	12/14 ���� ��, ��, ����Ʈ, ���, ŷ, �� �̵����� & ����ȯ(�ٵ� �̵��� �Ұ����� ��ġ������ �Ͽ����� �Է��� �߳����� ��������)���� Ŭ����


//	
// 
// 
// �����Ѱ�: board����, ü�� �̵�����, ������� Ÿ�̸ӱ����ؼ� ��ü����, �÷��̾� �̸��� ����Ʈ, king�� ����� �� �¸�
// 
// 
// 
//	���� �۾�
// 
//	--> 
//	--> ������� Ÿ�̸� ������ �׷����� ��� ������ ���û���

// 
#include <iostream>
#include <string>
#include <ctime>
#include <thread>
//	#include <utility>
#include <chrono>
#include <atomic>
#include <cstdlib>

using namespace std;

template <typename T1, typename T2>
struct SimplePair {
	T1 first;
	T2 second;

	SimplePair(T1 a, T2 b) : first(a), second(b){}
	SimplePair(): first(), second() {}
};

template <typename T1, typename T2>
SimplePair<T1, T2> makePair(T1 a, T2 b) {
	return SimplePair<T1, T2>(a, b);
}

template <typename T>
class vector {
private:
	int sz;
	T* elem;
	int space;

	void reserve(int newalloc) {
		if (newalloc <= space)
			return;

		T* p = new T[newalloc];

		for (int i = 0; i < sz; i++)
			p[i] = elem[i];

		delete[] elem;

		elem = p;
		space = newalloc;
	}

public:
	vector() : sz(0), elem(nullptr), space(0) {}
	explicit vector(int s) : sz(s), elem(new T[s]), space(s) {
		for (int i = 0; i < sz; i++) {
			elem[i] = T();
		}
	}
	explicit vector(int s, const T& d) : sz(s), elem(new T[s]), space(s) {
		for (int i = 0; i < sz; i++) {
			elem[i] = d;
		}
	}

	//	Ŭ���� ���ο� �����迭�� ����ϴ� ��� 
	//	�Ҹ���, ���� ������, ���Կ����ڸ� �ʼ��� ������ ��
	//	�̵� ������, �̵� ���Կ����ڴ� ���������
	~vector() {
		delete[] elem;
	}


	vector(const vector& a) : sz(a.sz), elem(new T[a.sz]), space(a.space) {
		//	copy constructor
		for (int i = 0; i < sz; i++)
			elem[i] = a.elem[i];

	}
	//	�����ڱ��� �ٵ�� �� ����

	vector& operator=(const vector& a) {
		//	copy assignment
		if (this == &a)
			return *this;

		if (a.sz <= space) {
			for (int i = 0; i < a.sz; i++)
				elem[i] = a.elem[i];
			sz = a.sz;
			return *this;
		}

		T* p = new T[a.sz];
		for (int i = 0; i < a.sz; i++)
			p[i] = a.elem[i];
		delete[] elem;
		sz = a.sz;
		space = a.sz;
		elem = p;
		return *this;
	}



	void resize(int newsize, const T& newelem) {
		if (newsize > sz) {
			reserve(newsize);

			for (int i = sz; i < newsize; i++)
				elem[i] = newelem;

		}
		sz = newsize;
	}

	void push_back(const T& d) {
		if (space == 0)
			reserve(8);
		else if (sz == space)
			reserve(2 * space);

		elem[sz] = d;
		++sz;
	}

	void erase(T* pos) {
		//	pos�� 3�� ����
		if (pos < elem || pos >= elem + sz)
			throw runtime_error("������ ������ϴ�.\n");

		int index = pos - elem;

		for (int i = index; i < sz - 1; i++) {
			elem[i] = elem[i + 1];


		}

		--sz;
	}

	//T get(int n) { return elem[n]; }
	/*void set(int n, const T& v) { elem[n] = v; }*/
	T& operator[](int n) { return elem[n]; }
	const T& operator[](int n) const { return elem[n]; }
	int size() const { return sz; }
	int capacity() const { return space; }
	T* begin() {
		return elem;
	}

	void clear() {
		for (int i = 0; i < sz; i++) {
			elem[i] = T();
		}

		sz = 0;
	}


};

struct Player {
	string name;
	int point = 0;

};

enum class PieceState
{
	NONE = -1, KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN
};

enum class PieceColor 
{
	NONE = -1, WHITE, BLACK
};

class ChessBoard {
private:
	vector<vector<SimplePair<PieceColor, PieceState>>> board;
	unsigned char numRows;
	unsigned char numColumns;
	int turn = 0;

public:
	ChessBoard() : numRows(8), numColumns(8) {
		board.clear();
		board.resize(numRows, vector<SimplePair<PieceColor, PieceState>>(numColumns, makePair(PieceColor::NONE, PieceState::NONE)));
		for (int i = 0; i < numRows; i++) {
			for (int j = 0; j < numColumns; j++) {
				board[i][j].first = PieceColor::NONE;
				board[i][j].second = PieceState::NONE;
			}
		}
	}

	void changeTurn(const Player& player1, const Player& player2) {
		turn  = 1 - turn;

		cout << "\n���� ����Ǿ����ϴ�. ���� ��: " << (turn == 0 ? player1.name : player2.name) << endl;
		cout << "�̵��� ��ġ�� �ٽ� �Է��ϼ���(�� 3454, ����: q): ";
	}
	void initialize();
	int getTurn() {
		return turn;

	}
	char getStateLetter(const PieceState& state) const;

	void movePiece(char input1, char input2, char result1, char result2, Player& player1, Player& player2) {
		if (!getTurn()) {
			if (checkConditionWhite(input1, input2, result1, result2)) {
				if (board[result1][result2].second == PieceState::PAWN)
					player1.point += 1;
				else if (board[result1][result2].second == PieceState::ROOK)
					player1.point += 2;
				else if (board[result1][result2].second == PieceState::KNIGHT)
					player1.point += 2;
				else if (board[result1][result2].second == PieceState::BISHOP)
					player1.point += 2;
				else if (board[result1][result2].second == PieceState::QUEEN)
					player1.point += 5;
				else if (board[result1][result2].second == PieceState::KING) {
					player1.point += 10;
					cout << "ŷ�� ��ҽ��ϴ�. " << player1.name << "�� �¸��Դϴ�!!" << endl;
					exit(0);
				}
				board[result1][result2].first = board[input1][input2].first;
				board[result1][result2].second = board[input1][input2].second;
				board[input1][input2].first = PieceColor::NONE;
				board[input1][input2].second = PieceState::NONE;
				return;
			}
			else
			{
				cout << "�̵��� �Ұ����� ��ġ�Դϴ�\n";
				return;
			}
		}
		else {

			if (checkConditionBlack(input1, input2, result1, result2)) {
				if (board[result1][result2].second == PieceState::PAWN)
					player2.point += 1;
				else if (board[result1][result2].second == PieceState::ROOK)
					player2.point += 2;
				else if (board[result1][result2].second == PieceState::KNIGHT)
					player2.point += 2;
				else if (board[result1][result2].second == PieceState::BISHOP)
					player2.point += 2;
				else if (board[result1][result2].second == PieceState::QUEEN)
					player2.point += 5;
				else if (board[result1][result2].second == PieceState::KING) {
					player2.point += 10;
					cout << "ŷ�� ��ҽ��ϴ�. " << player2.name << "�� �¸��Դϴ�!!" << endl;
					exit(0);
				}
				board[result1][result2].first = board[input1][input2].first;
				board[result1][result2].second = board[input1][input2].second;
				board[input1][input2].first = PieceColor::NONE;
				board[input1][input2].second = PieceState::NONE;
				return;
			}
			else
			{
				cout << "�̵��� �Ұ����� ��ġ�Դϴ�\n";
				return;
			}
		}
	}


	bool checkConditionWhite(char input1, char input2, char result1, char result2) {
		//	�Է¹��� ��ġ�� �⹰�� �ش��ϴ� ĭ���� �̵��� �� �ִ����� Ȯ���Ԥ���

		
			turn = 1;
		

		if (result1 == input1 && result2 == input2) 
			//	���� ��ġ�� ���� ���� false ������
			return false;
		

		if (result1 >= 8 || result2 >= 8 || input1 >= 8 || input2 >= 8)
			//	�������� ������� ���带 ����� ��� 
			return false;

		if (board[input1][input2].first == PieceColor::WHITE) {
			if (board[input1][input2].second == PieceState::PAWN) {
				//	���� �⺻������ �� �� �ִ� �� ������ 3ĭ�� ��쿡 ���ؼ� Ȯ����

				if (result1 == input1 - 1 && result2 == input2) {
					//	����
					if (board[result1][result2].first == PieceColor::NONE) {
						return true;
					}
					else {
						return false;
					}
				}
				else if (result1 == input1 - 1 && result2 == input2 - 1) {
					//	������
					if (board[result1][result2].first == PieceColor::BLACK)
						return true;
					else
						return false;
				}
				else if (result1 == input1 - 1 && result2 == input2 + 1) {
					//	��������
					if (board[result1][result2].first == PieceColor::BLACK)
						return true;
					else
						return false;
				}
				else
					return false;
			}

			else if (board[input1][input2].second == PieceState::ROOK) {
				//	���� �����¿� �������� �� �� ������ ������̳� �ٸ� �⹰�� ���θ����� �ű������ �� �� ����
				//	��Ȯ�ϰԴ� ���θ��� �ٷ��� ���� �̵��� �� �ִٰ� ǥ���ϰ� ����� �⹰�� ���θ��� ���� �� ĭ �� �̵��� �� �ְ�
				char up = 0;
				char down = 0;
				char right = 0;
				char left = 0;

				for (int i = 1; i < 9; i++) {
					if (input1 - i < 0) {
						up = input1 - 1 + 1;
						break;
					}
					if (board[input1 - i][input2].first != PieceColor::NONE) {
						up = input1 - i;
						break;
					}
				}

				if (board[up][input2].first == PieceColor::WHITE) {
					if (up != input1) {
						up += 1;
					}
				}

				for (int i = 1; i < 9; i++) {
					if (input1 + i > 7) {
						down = input1 + i - 1;
						break;
					}
					if (board[input1 + i][input2].first != PieceColor::NONE) {
						down = input1 + i;
						break;
					}
				}

				if (board[down][input2].first == PieceColor::WHITE) {
					if (down != input1) {
						down -= 1;
					}
				}

				for (int i = 1; i < 9; i++) {
					if (input2 + i > 7) {
						right = input2 + i - 1;
						break;
					}
					if (board[input1][input2 + i].first != PieceColor::NONE) {
						right = input2 + i;
						break;
					}
				}

				if (board[input1][right].first == PieceColor::WHITE) {
					if (input2 != right) {
						right -= 1;
					}
				}

				for (int i = 1; i < 9; i++) {
					if (input2 - i < 0) {
						left = input2 - i + 1;
						break;
					}
					if (board[input1][input2 - i].first != PieceColor::NONE) {
						left = input2 - i;
						break;
					}
				}

				if (board[input1][left].first == PieceColor::WHITE) {
					if (input2 != left) {
						left += 1;
					}
				}

				

				if (result1 >= up && result1 <= down) {
					if (result2 <= right && result2 >= left) {	
							return true;
					}
					else {
						return false;
					}
				}
				else {
					return false;
				}
			}
		
			else if (board[input1][input2].second == PieceState::KNIGHT) {

				if (result1 == input1 - 2 && result2 == input2 + 1 && board[result1][result2].first != PieceColor::WHITE)
					//	��������
					return true;
				else if (result1 == input1 - 2 && result2 == input2 - 1 && board[result1][result2].first != PieceColor::WHITE)
					//	������
					return true;
				else if (result1 == input1 - 1 && result2 == input2 + 2 && board[result1][result2].first != PieceColor::WHITE)
					//	��������
					return true;
				else if (result1 == input1 + 1 && result2 == input2 + 2 && board[result1][result2].first != PieceColor::WHITE)
					//	�����ʾƷ�
					return true;
				else if (result1 == input1 + 2 && result2 == input2 + 1 && board[result1][result2].first != PieceColor::WHITE)
					//	�Ʒ�������
					return true;
				else if (result1 == input1 + 2 && result2 == input2 - 1 && board[result1][result2].first != PieceColor::WHITE)
					//	�Ʒ�����
					return true;
				else if (result1 == input1 - 1 && result2 == input2 - 2 && board[result1][result2].first != PieceColor::WHITE)
					//	������
					return true;
				else if (result1 == input1 + 1 && result2 == input2 - 2 && board[result1][result2].first != PieceColor::WHITE)
					//	���ʾƷ�
					return true;
				else
					return false;
			}
			else if (board[input1][input2].second == PieceState::BISHOP) {
				//	��� ��� ������������ �밢������ �������� ������ �� �� �ִ� ���� ��� �� �ΰ��� ������ �����ؾ� ��
				char upright1 = input1, upright2 = input2;
				char upleft1 = input1, upleft2 = input2;
				char downright1 = input1, downright2 = input2;
				char downleft1 = input1, downleft2 = input2;

				for (int i = 1; i < 9; i++) {
					//	����� ���� �ƴ϶� �����̶�
					//	�ݺ��� �������鼭 �迭 �ٱ����� ���������� �ʰ� �ؾ���
					//	�������� �밢��
					if (upright1 - i < 0 || upright2 + i > 7) {
						upright1 = input1 - i + 1;
						upright2 = input2 + i - 1;
						break;
					}

					if (board[input1 - i][input2 + i].first != PieceColor::NONE) {
						upright1 = input1 - i;
						upright2 = input2 + i;
						break;
					}
				}

				if (board[upright1][upright2].first == PieceColor::WHITE) {
					if (input1 != upright1 && input2 != upright2) {
						upright1 += 1;
						upright2 -= 1;
					}
				}

				for (int i = 1; i < 9; i++) {
				
					if (upleft1 - i < 0 || upleft2 - i < 0) {
						upleft1 = input1 - i + 1;
						upleft2 = input2 - i + 1;
						break;
					}

					if (board[input1 - i][input2 - i].first != PieceColor::NONE) {
						upleft1 = input1 - i;
						upleft2 = input2 - i;
						break;
					}
				}

				if (board[upleft1][upleft2].first == PieceColor::WHITE) {
					if (input1 != upleft1 && input2 != upleft2) {
						upleft1 += 1;
						upleft2 += 1;
					}
				}
				

				for (int i = 1; i < 9; i++) {
	
					if (downright1 + i > 7 || downright2 + i > 7) {
						downright1 = input1 + i - 1;
						downright2 = input2 + i - 1;
						break;
					}

					if (board[input1 + i][input2 + i].first != PieceColor::NONE) {
						downright1 = input1 + i;
						downright2 = input2 + i;
						break;
					}
				}

				if (board[downright1][downright2].first == PieceColor::WHITE) {
					if (input1 != downright1 && input2 != downright2) {
						downright1 -= 1;
						downright2 -= 1;
					}
				}
				for (int i = 1; i < 9; i++) {
					if (downleft1 + i > 7 || downleft2 - i < 0) {
						downleft1 = input1 + i - 1;
						downleft2 = input2 - i + 1;
						break;
					}

					if (board[input1 + i][input2 - i].first != PieceColor::NONE) {
						downleft1 = input1 + i;
						downleft2 = input2 - i;
						break;
					}
				}

				if (board[downleft1][downleft2].first == PieceColor::WHITE) {
					if (input1 != downleft1 && input2 != downleft2) {
						downleft1 -= 1;
						downleft2 += 1;
					}
				}

				if (result1 < input1 && result2 > input2) {
					//	 upright
					if (result1 >= upright1 && result2 <= upright2)
						return true;
					else
						return false;
				}

				if (result1 < input1 && result2 < input2) {
					//	 upleft
					if (result1 >= upleft1 && result2 >= upleft2)
						return true;
					else
						return false;
				}

				if (result1 > input1 && result2 > input2) {
					//	 downright
					if (result1 <= downright1 && result2 <= downright2)
						return true;
					else
						return false;
				}

				if (result1 > input1 && result2 < input2) {
					//	 downleft
					if (result1 <= downleft1 && result2 >= downleft2)
						return true;
					else
						return false;
				}

			
			}
			else if (board[input1][input2].second == PieceState::QUEEN) {
				char up = 0;
				char down = 0;
				char right = 0;
				char left = 0;
				char upright1 = input1, upright2 = input2;
				char upleft1 = input1, upleft2 = input2;
				char downright1 = input1, downright2 = input2;
				char downleft1 = input1, downleft2 = input2;


				for (int i = 1; i < 9; i++) {
					if (input1 - i < 0) {
						up = input1 - 1 + 1;
						break;
					}
					if (board[input1 - i][input2].first != PieceColor::NONE) {
						up = input1 - i;
						break;
					}
				}

				if (board[up][input2].first == PieceColor::WHITE) {
					if (up != input1) {
						up += 1;
					}
				}

				for (int i = 1; i < 9; i++) {
					if (input1 + i > 7) {
						down = input1 + i - 1;
						break;
					}
					if (board[input1 + i][input2].first != PieceColor::NONE) {
						down = input1 + i;
						break;
					}
				}

				if (board[down][input2].first == PieceColor::WHITE) {
					if (down != input1) {
						down -= 1;
					}
				}

				for (int i = 1; i < 9; i++) {
					if (input2 + i > 7) {
						right = input2 + i - 1;
						break;
					}
					if (board[input1][input2 + i].first != PieceColor::NONE) {
						right = input2 + i;
						break;
					}
				}

				if (board[input1][right].first == PieceColor::WHITE) {
					if (input2 != right) {
						right -= 1;
					}
				}

				for (int i = 1; i < 9; i++) {
					if (input2 - i < 0) {
						left = input2 - i + 1;
						break;
					}
					if (board[input1][input2 - i].first != PieceColor::NONE) {
						left = input2 - i;
						break;
					}
				}

				if (board[input1][left].first == PieceColor::WHITE) {
					if (input2 != left) {
						left += 1;
					}
				}

			
				for (int i = 1; i < 9; i++) {
					if (upright1 - i < 0 || upright2 + i > 7) {
						upright1 = input1 - i + 1;
						upright2 = input2 + i - 1;
						break;
					}

					if (board[input1 - i][input2 + i].first != PieceColor::NONE) {
						upright1 = input1 - i;
						upright2 = input2 + i;
						break;
					}
				}

				if (board[upright1][upright2].first == PieceColor::WHITE) {
					if (input1 != upright1 && input2 != upright2) {
						upright1 += 1;
						upright2 -= 1;
					}
				}

				for (int i = 1; i < 9; i++) {

					if (upleft1 - i < 0 || upleft2 - i < 0) {
						upleft1 = input1 - i + 1;
						upleft2 = input2 - i + 1;
						break;
					}

					if (board[input1 - i][input2 - i].first != PieceColor::NONE) {
						upleft1 = input1 - i;
						upleft2 = input2 - i;
						break;
					}
				}

				if (board[upleft1][upleft2].first == PieceColor::WHITE) {
					if (input1 != upleft1 && input2 != upleft2) {
						upleft1 += 1;
						upleft2 += 1;
					}
				}


				for (int i = 1; i < 9; i++) {

					if (downright1 + i > 7 || downright2 + i > 7) {
						downright1 = input1 + i - 1;
						downright2 = input2 + i - 1;
						break;
					}

					if (board[input1 + i][input2 + i].first != PieceColor::NONE) {
						downright1 = input1 + i;
						downright2 = input2 + i;
						break;
					}
				}

				if (board[downright1][downright2].first == PieceColor::WHITE) {
					if (input1 != downright1 && input2 != downright2) {
						downright1 -= 1;
						downright2 -= 1;
					}
				}
				for (int i = 1; i < 9; i++) {
					if (downleft1 + i > 7 || downleft2 - i < 0) {
						downleft1 = input1 + i - 1;
						downleft2 = input2 - i + 1;
						break;
					}

					if (board[input1 + i][input2 - i].first != PieceColor::NONE) {
						downleft1 = input1 + i;
						downleft2 = input2 - i;
						break;
					}
				}

				if (board[downleft1][downleft2].first == PieceColor::WHITE) {
					if (input1 != downleft1 && input2 != downleft2) {
						downleft1 -= 1;
						downleft2 += 1;
					}
				}

				if (result1 < input1 && result2 == input2) {
					//	��
					if (result1 >= up )
						return true;
					else
						return false;
				}
				if (result1 > input1 && result2 == input2) {
					//	�Ʒ�
					if (result1 <= down)
						return true;
					else
						return false;
				}
				if (result1 == input1 && result2 < input2) {
					//	����
					if (result2 >= left)
						return true;
					else
						return false;
				}
				if (result1 == input1 && result2 > input2) {
					//	������
					if (result2 <= right)
						return true;
					else
						return false;
				}


				if (result1 < input1 && result2 > input2) {
					//	 upright
					if (result1 >= upright1 && result2 <= upright2)
						return true;
					else
						return false;
				}

				if (result1 < input1 && result2 < input2) {
					//	 upleft
					if (result1 >= upleft1 && result2 >= upleft2)
						return true;
					else
						return false;
				}

				if (result1 > input1 && result2 > input2) {
					//	 downright
					if (result1 <= downright1 && result2 <= downright2)
						return true;
					else
						return false;
				}

				if (result1 > input1 && result2 < input2) {
					//	 downleft
					if (result1 <= downleft1 && result2 >= downleft2)
						return true;
					else
						return false;
				}











			}
			else if (board[input1][input2].second == PieceState::KING) {
				if (result1 == input1 - 1 && result2 == input2) {
					//	����
					if (board[result1][result2].first != PieceColor::WHITE) {
						return true;
					}
					else {
						return false;
					}
				}
				else if (result1 == input1 + 1 && result2 == input2) {
					//	�ڷ�
					if (board[result1][result2].first != PieceColor::WHITE)
						return true;
					else
						return false;
				}
				else if (result1 == input1 && result2 == input2 - 1) {
					//	����
					if (board[result1][result2].first != PieceColor::WHITE)
						return true;
					else
						return false;
				}
				else if (result1 == input1 && result2 == input2 + 1) {
					//	������
					if (board[result1][result2].first != PieceColor::WHITE)
						return true;
					else
						return false;
				}
				else if (result1 == input1 - 1 && result2 == input2 + 1) {
					//	��������
					if (board[result1][result2].first != PieceColor::WHITE)
						return true;
					else
						return false;
				}
				else if (result1 == input1 - 1 && result2 == input2 - 1) {
					//	������
					if (board[result1][result2].first != PieceColor::WHITE)
						return true;
					else
						return false;
				}
				else if (result1 == input1 + 1 && result2 == input2 + 1) {
					//	�Ʒ�������
					if (board[result1][result2].first != PieceColor::WHITE)
						return true;
					else
						return false;
				}
				else if (result1 == input1 + 1 && result2 == input2 - 1) {
					//	�Ʒ�����
					if (board[result1][result2].first != PieceColor::WHITE)
						return true;
					else
						return false;
				}
				else
					return false;
			}
			else {
				return false;
			}
		}
		else if(board[input1][input2].first == PieceColor::BLACK) {
			cout << endl << "�ڽ��� ���� �ƴմϴ�" << endl;
			turn = 0;
			return false;
			}

		else {
			return false;
		}
	}

	bool checkConditionBlack(char input1, char input2, char result1, char result2) {
		//	�Է¹��� ��ġ�� �⹰�� �ش��ϴ� ĭ���� �̵��� �� �ִ����� Ȯ����

		
			turn = 0;
		

		if (result1 == input1 && result2 == input2)
			//	���� ��ġ�� ���� ���� false ������
			return false;


		if (result1 >= 8 || result2 >= 8 || input1 >= 8 || input2 >= 8)
			//	�������� ������� ���带 ����� ��� 
			return false;

		if (board[input1][input2].first == PieceColor::BLACK) {
			if (board[input1][input2].second == PieceState::PAWN) {
				//	���� �⺻������ �� �� �ִ� �� ������ 3ĭ�� ��쿡 ���ؼ� Ȯ����

				if (result1 == input1 + 1 && result2 == input2) {
					//	����
					if (board[result1][result2].first == PieceColor::NONE) {
						return true;
					}
					else {
						return false;
					}
				}
				else if (result1 == input1 + 1 && result2 == input2 - 1) {
					//	�Ʒ�����
					if (board[result1][result2].first == PieceColor::WHITE)
						return true;
					else
						return false;
				}
				else if (result1 == input1 + 1 && result2 == input2 + 1) {
					//	�Ʒ�������
					if (board[result1][result2].first == PieceColor::WHITE)
						return true;
					else
						return false;
				}
				else
					return false;
			}

			else if (board[input1][input2].second == PieceState::ROOK) {
				//	���� �����¿� �������� �� �� ������ ������̳� �ٸ� �⹰�� ���θ����� �ű������ �� �� ����
				//	��Ȯ�ϰԴ� ���θ��� �ٷ��� ���� �̵��� �� �ִٰ� ǥ���ϰ� ����� �⹰�� ���θ��� ���� �� ĭ �� �̵��� �� �ְ�
				char up = 0;
				char down = 0;
				char right = 0;
				char left = 0;

				for (int i = 1; i < 9; i++) {
					if (input1 - i < 0) {
						up = input1 - 1 + 1;
						break;
					}
					if (board[input1 - i][input2].first != PieceColor::NONE) {
						up = input1 - i;
						break;
					}
				}

				if (board[up][input2].first == PieceColor::BLACK) {
					if (up != input1) {
						up += 1;
					}
				}

				for (int i = 1; i < 9; i++) {
					if (input1 + i > 7) {
						down = input1 + i - 1;
						break;
					}
					if (board[input1 + i][input2].first != PieceColor::NONE) {
						down = input1 + i;
						break;
					}
				}

				if (board[down][input2].first == PieceColor::BLACK) {
					if (down != input1) {
						down -= 1;
					}
				}

				for (int i = 1; i < 9; i++) {
					if (input2 + i > 7) {
						right = input2 + i - 1;
						break;
					}
					if (board[input1][input2 + i].first != PieceColor::NONE) {
						right = input2 + i;
						break;
					}
				}

				if (board[input1][right].first == PieceColor::BLACK) {
					if (input2 != right) {
						right -= 1;
					}
				}

				for (int i = 1; i < 9; i++) {
					if (input2 - i < 0) {
						left = input2 - i + 1;
						break;
					}
					if (board[input1][input2 - i].first != PieceColor::NONE) {
						left = input2 - i;
						break;
					}
				}

				if (board[input1][left].first == PieceColor::BLACK) {
					if (input2 != left) {
						left += 1;
					}
				}



				if (result1 >= up && result1 <= down) {
					if (result2 <= right && result2 >= left) {
						return true;
					}
					else {
						return false;
					}
				}
				else {
					return false;
				}
			}

			else if (board[input1][input2].second == PieceState::KNIGHT) {

				if (result1 == input1 - 2 && result2 == input2 + 1 && board[result1][result2].first != PieceColor::BLACK)
					//	��������
					return true;
				else if (result1 == input1 - 2 && result2 == input2 - 1 && board[result1][result2].first != PieceColor::BLACK)
					//	������
					return true;
				else if (result1 == input1 - 1 && result2 == input2 + 2 && board[result1][result2].first != PieceColor::BLACK)
					//	��������
					return true;
				else if (result1 == input1 + 1 && result2 == input2 + 2 && board[result1][result2].first != PieceColor::BLACK)
					//	�����ʾƷ�
					return true;
				else if (result1 == input1 + 2 && result2 == input2 + 1 && board[result1][result2].first != PieceColor::BLACK)
					//	�Ʒ�������
					return true;
				else if (result1 == input1 + 2 && result2 == input2 - 1 && board[result1][result2].first != PieceColor::BLACK)
					//	�Ʒ�����
					return true;
				else if (result1 == input1 - 1 && result2 == input2 - 2 && board[result1][result2].first != PieceColor::BLACK)
					//	������
					return true;
				else if (result1 == input1 + 1 && result2 == input2 - 2 && board[result1][result2].first != PieceColor::BLACK)
					//	���ʾƷ�
					return true;
				else
					return false;
			}
			else if (board[input1][input2].second == PieceState::BISHOP) {
				//	��� ��� ������������ �밢������ �������� ������ �� �� �ִ� ���� ��� �� �ΰ��� ������ �����ؾ� ��
				char upright1 = input1, upright2 = input2;
				char upleft1 = input1, upleft2 = input2;
				char downright1 = input1, downright2 = input2;
				char downleft1 = input1, downleft2 = input2;

				for (int i = 1; i < 9; i++) {
					//	����� ���� �ƴ϶� �����̶�
					//	�ݺ��� �������鼭 �迭 �ٱ����� ���������� �ʰ� �ؾ���
					//	�������� �밢��
					if (upright1 - i < 0 || upright2 + i > 7) {
						upright1 = input1 - i + 1;
						upright2 = input2 + i - 1;
						break;
					}

					if (board[input1 - i][input2 + i].first != PieceColor::NONE) {
						upright1 = input1 - i;
						upright2 = input2 + i;
						break;
					}
				}

				if (board[upright1][upright2].first == PieceColor::BLACK) {
					if (input1 != upright1 && input2 != upright2) {
						upright1 += 1;
						upright2 -= 1;
					}
				}

				for (int i = 1; i < 9; i++) {

					if (upleft1 - i < 0 || upleft2 - i < 0) {
						upleft1 = input1 - i + 1;
						upleft2 = input2 - i + 1;
						break;
					}

					if (board[input1 - i][input2 - i].first != PieceColor::NONE) {
						upleft1 = input1 - i;
						upleft2 = input2 - i;
						break;
					}
				}

				if (board[upleft1][upleft2].first == PieceColor::BLACK) {
					if (input1 != upleft1 && input2 != upleft2) {
						upleft1 += 1;
						upleft2 += 1;
					}
				}

				
				for (int i = 1; i < 9; i++) {

					if (downright1 + i > 7 || downright2 + i > 7) {
						downright1 = input1 + i - 1;
						downright2 = input2 + i - 1;
						break;
					}

					if (board[input1 + i][input2 + i].first != PieceColor::NONE) {
						downright1 = input1 + i;
						downright2 = input2 + i;
						break;
					}
				}

				if (board[downright1][downright2].first == PieceColor::BLACK) {
					if (input1 != downright1 && input2 != downright2) {
						downright1 -= 1;
						downright2 -= 1;
					}
				}
				for (int i = 1; i < 9; i++) {
					if (downleft1 + i > 7 || downleft2 - i < 0) {
						downleft1 = input1 + i - 1;
						downleft2 = input2 - i + 1;
						break;
					}

					if (board[input1 + i][input2 - i].first != PieceColor::NONE) {
						downleft1 = input1 + i;
						downleft2 = input2 - i;
						break;
					}
				}

				if (board[downleft1][downleft2].first == PieceColor::BLACK) {
					if (input1 != downleft1 && input2 != downleft2) {
						downleft1 -= 1;
						downleft2 += 1;
					}
				}

				if (result1 < input1 && result2 > input2) {
					//	upright
					if (result1 >= upright1 && result2 <= upright2)
						return true;
					else
						return false;
				}

				if (result1 < input1 && result2 < input2) {
					//	opleft
					if (result1 >= upleft1 && result2 >= upleft2)
						return true;
					else
						return false;
				}

				if (result1 > input1 && result2 > input2) {
					//	downright
					if (result1 <= downright1 && result2 <= downright2)
						return true;
					else
						return false;
				}

				if (result1 > input1 && result2 < input2) {
					//	downleft
					if (result1 <= downleft1 && result2 >= downleft2)
						return true;
					else
						return false;
				}


			}
			else if (board[input1][input2].second == PieceState::QUEEN) {
				int up = input1;
				int down = input1;
				int right = input2;
				int left = input2;

				for (int i = 1; i < 9; i++) {
					if (input1 - i < 0) {
						up = input1 - 1 + 1;
						break;
					}
					if (board[input1 - i][input2].first != PieceColor::NONE) {
						up = input1 - i;
						break;
					}
				}

				if (board[up][input2].first == PieceColor::BLACK) {
					if (up != input1) {
						up += 1;
					}
				}

				for (int i = 1; i < 9; i++) {
					if (input1 + i > 7) {
						down = input1 + i - 1;
						break;
					}
					if (board[input1 + i][input2].first != PieceColor::NONE) {
						down = input1 + i;
						break;
					}
				}

				if (board[down][input2].first == PieceColor::BLACK) {
					if (down != input1) {
						down -= 1;
					}
				}

				for (int i = 1; i < 9; i++) {
					if (input2 + i > 7) {
						right = input2 + i - 1;
						break;
					}
					if (board[input1][input2 + i].first != PieceColor::NONE) {
						right = input2 + i;
						break;
					}
				}

				if (board[input1][right].first == PieceColor::BLACK) {
					if (input2 != right) {
						right -= 1;
					}
				}

				for (int i = 1; i < 9; i++) {
					if (input2 - i < 0) {
						left = input2 - i + 1;
						break;
					}
					if (board[input1][input2 - i].first != PieceColor::NONE) {
						left = input2 - i;
						break;
					}
				}

				if (board[input1][left].first == PieceColor::BLACK) {
					if (input2 != left) {
						left += 1;
					}
				}

				char upright1 = input1, upright2 = input2;
				char upleft1 = input1, upleft2 = input2;
				char downright1 = input1, downright2 = input2;
				char downleft1 = input1, downleft2 = input2;

				for (int i = 1; i < 9; i++) {
					if (upright1 - i < 0 || upright2 + i > 7) {
						upright1 = input1 - i + 1;
						upright2 = input2 + i - 1;
						break;
					}

					if (board[input1 - i][input2 + i].first != PieceColor::NONE) {
						upright1 = input1 - i;
						upright2 = input2 + i;
						break;
					}
				}

				if (board[upright1][upright2].first == PieceColor::WHITE) {
					if (input1 != upright1 && input2 != upright2) {
						upright1 += 1;
						upright2 -= 1;
					}
				}

				for (int i = 1; i < 9; i++) {

					if (upleft1 - i < 0 || upleft2 - i < 0) {
						upleft1 = input1 - i + 1;
						upleft2 = input2 - i + 1;
						break;
					}

					if (board[input1 - i][input2 - i].first != PieceColor::NONE) {
						upleft1 = input1 - i;
						upleft2 = input2 - i;
						break;
					}
				}

				if (board[upleft1][upleft2].first == PieceColor::WHITE) {
					if (input1 != upleft1 && input2 != upleft2) {
						upleft1 += 1;
						upleft2 += 1;
					}
				}


				for (int i = 1; i < 9; i++) {

					if (downright1 + i > 7 || downright2 + i > 7) {
						downright1 = input1 + i - 1;
						downright2 = input2 + i - 1;
						break;
					}

					if (board[input1 + i][input2 + i].first != PieceColor::NONE) {
						downright1 = input1 + i;
						downright2 = input2 + i;
						break;
					}
				}

				if (board[downright1][downright2].first == PieceColor::WHITE) {
					if (input1 != downright1 && input2 != downright2) {
						downright1 -= 1;
						downright2 -= 1;
					}
				}
				for (int i = 1; i < 9; i++) {
					if (downleft1 + i > 7 || downleft2 - i < 0) {
						downleft1 = input1 + i - 1;
						downleft2 = input2 - i + 1;
						break;
					}

					if (board[input1 + i][input2 - i].first != PieceColor::NONE) {
						downleft1 = input1 + i;
						downleft2 = input2 - i;
						break;
					}
				}

				if (board[downleft1][downleft2].first == PieceColor::WHITE) {
					if (input1 != downleft1 && input2 != downleft2) {
						downleft1 -= 1;
						downleft2 += 1;
					}
				}

				if (result1 < input1 && result2 == input2) {
					//	��
					if (result1 >= up)
						return true;
					else
						return false;
				}
				if (result1 > input1 && result2 == input2) {
					//	�Ʒ�
					if (result1 <= down)
						return true;
					else
						return false;
				}
				if (result1 == input1 && result2 < input2) {
					//	����
					if (result2 >= left)
						return true;
					else
						return false;
				}
				if (result1 == input1 && result2 > input2) {
					//	������
					if (result2 <= right)
						return true;
					else
						return false;
				}


				if (result1 < input1 && result2 > input2) {
					//	 upright
					if (result1 >= upright1 && result2 <= upright2)
						return true;
					else
						return false;
				}

				if (result1 < input1 && result2 < input2) {
					//	 upleft
					if (result1 >= upleft1 && result2 >= upleft2)
						return true;
					else
						return false;
				}

				if (result1 > input1 && result2 > input2) {
					//	 downright
					if (result1 <= downright1 && result2 <= downright2)
						return true;
					else
						return false;
				}

				if (result1 > input1 && result2 < input2) {
					//	 downleft
					if (result1 <= downleft1 && result2 >= downleft2)
						return true;
					else
						return false;
				}










			}
			else if (board[input1][input2].second == PieceState::KING) {
				if (result1 == input1 - 1 && result2 == input2) {
					//	����
					if (board[result1][result2].first != PieceColor::BLACK) {
						return true;
					}
					else {
						return false;
					}
				}
				else if (result1 == input1 + 1 && result2 == input2) {
					//	�ڷ�
					if (board[result1][result2].first != PieceColor::BLACK)
						return true;
					else
						return false;
				}
				else if (result1 == input1 && result2 == input2 - 1) {
					//	����
					if (board[result1][result2].first != PieceColor::BLACK)
						return true;
					else
						return false;
				}
				else if (result1 == input1 && result2 == input2 + 1) {
					//	������
					if (board[result1][result2].first != PieceColor::BLACK)
						return true;
					else
						return false;
				}
				else if (result1 == input1 - 1 && result2 == input2 + 1) {
					//	��������
					if (board[result1][result2].first != PieceColor::WHITE)
						return true;
					else
						return false;
				}
				else if (result1 == input1 - 1 && result2 == input2 - 1) {
					//	������
					if (board[result1][result2].first != PieceColor::WHITE)
						return true;
					else
						return false;
				}
				else if (result1 == input1 + 1 && result2 == input2 + 1) {
					//	�Ʒ�������
					if (board[result1][result2].first != PieceColor::WHITE)
						return true;
					else
						return false;
				}
				else if (result1 == input1 + 1 && result2 == input2 - 1) {
					//	�Ʒ�����
					if (board[result1][result2].first != PieceColor::WHITE)
						return true;
					else
						return false;
				}
				else
					return false;
			}
			else {
				return false;
			}
		}
		else if (board[input1][input2].first == PieceColor::WHITE) {
			cout << "�ڽ��� ���� �ƴմϴ�" << endl;
			turn = 1;

			return false;
		}
		else {
			return false;
		}
	}


	

	void printBoard() const {
		for (int i = 0; i < numRows; i++) {
			for (int j = 0; j < numColumns; j++) {
				cout << getStateLetter(board[i][j].second) << " ";
			}
			cout << endl;
		}
	}

	 


};

char ChessBoard::getStateLetter(const PieceState& state) const {
	char letter = ' ';

	switch (state) {
	case PieceState::NONE: letter = ' '; break;
	case PieceState::PAWN:  letter = '@'; break;
	case PieceState::KING: letter = '#'; break;
	case PieceState::QUEEN: letter = '*'; break;
	case PieceState::ROOK: letter = '%'; break;
	case PieceState::BISHOP: letter = '$'; break;
	case PieceState::KNIGHT: letter = '&'; break;


	}

	return letter;
}

void ChessBoard::initialize() {
	for (int i = 2; i < 5; i++) {
		for (int j = 0; j < numColumns; j++) {
			board[i][j].first = PieceColor::NONE;
			board[i][j].second = PieceState::NONE;
		}
	}

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < numColumns; j++) {
			board[i][j].first = PieceColor::BLACK;
		}
	}

	for (int i = 6; i < 8; i++) {
		for (int j = 0; j < numColumns; j++) {
			board[i][j].first = PieceColor::WHITE;
		}
	}

	for (int i = 0; i < numColumns; i++) {
		board[1][i].second = PieceState::PAWN;
	}

	for (int i = 0; i < numColumns; i++) {
		board[6][i].second = PieceState::PAWN;
	}

	board[0][0].second = PieceState::ROOK;
	board[0][1].second = PieceState::KNIGHT;
	board[0][2].second = PieceState::BISHOP;
	board[0][3].second = PieceState::QUEEN;
	board[0][4].second = PieceState::KING;
	board[0][5].second = PieceState::BISHOP;
	board[0][6].second = PieceState::KNIGHT;
	board[0][7].second = PieceState::ROOK;



	board[7][0].second = PieceState::ROOK;
	board[7][1].second = PieceState::KNIGHT;
	board[7][2].second = PieceState::BISHOP;
	board[7][3].second = PieceState::QUEEN;
	board[7][4].second = PieceState::KING;
	board[7][5].second = PieceState::BISHOP;
	board[7][6].second = PieceState::KNIGHT;
	board[7][7].second = PieceState::ROOK;
}

atomic<bool> working(true);
atomic<int> traceTime(0);

void setTimer(ChessBoard& board, const Player& player1, const Player& player2){
	while (working) {
		this_thread::sleep_for(chrono::seconds(1));
		traceTime++;
		

		if (traceTime >= 15&& working) {
			board.changeTurn(ref(player1), ref(player2));
			traceTime = 0;
		}
	}
}

int main() {
	ChessBoard chess1;
	Player player1, player2;

	cout << "�÷��̾� �θ��� �̸��� �Է��ϼ���: ";
	cin >> player1.name >> player2.name;

	thread thread1(setTimer, ref(chess1), ref(player1), ref(player2));

	chess1.initialize();
	chess1.printBoard();

	while (1) {
		int i = 0;
		char op[4] = "";

		cout << endl;
		cout << "���ϴ� �⹰�� ������ġ�� �̵��� ��ġ�� �Է��ϼ���(�� 3454, ����: q): ";

		for (int i = 0; i < 4; i++) {
			cin >> op[i];

			if (op[i] == 'q') {
				working = false;
				break;
			}
			op[i] -= '0';
		}

		if (!working) {
			break;
		}


		chess1.movePiece(op[0], op[1], op[2], op[3], ref(player1), ref(player2));

		cout << "���� ������ " << player1.name << "���� " << player1.point << ", " << player2.name << "���� " << player2.point << "�� ŉ���� �����Դϴ�." << endl;

		traceTime = 0;

		cout << endl << endl << endl;
		chess1.printBoard();

		
	}

		working = false;

		if (thread1.joinable()) {
			thread1.join();
		}

		cout << "������ ����Ǿ����ϴ�." << endl;

		return 0;
	}//	1