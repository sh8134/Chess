//	12/14 현재 폰, 룩, 나이트, 비숍, 킹, 퀸 이동조건 & 턴전환(근데 이동이 불가능한 위치넣으면 턴오류뜸 입력을 잘넣으면 괜찮은데)까지 클리어


//	
// 
// 
// 구현한것: board구현, 체스 이동조건, 스레드로 타이머구현해서 턴체인지, 플레이어 이름과 포인트, king을 잡았을 때 승리
// 
// 
// 
//	다음 작업
// 
//	--> 
//	--> 스레드로 타이머 구현과 그래픽적 요소 구현은 선택사항

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

	//	클래스 내부에 동적배열을 사용하는 경우 
	//	소멸자, 복사 생성자, 대입연산자를 필수로 만들어야 함
	//	이동 생성자, 이동 대입연산자는 선택적요소
	~vector() {
		delete[] elem;
	}


	vector(const vector& a) : sz(a.sz), elem(new T[a.sz]), space(a.space) {
		//	copy constructor
		for (int i = 0; i < sz; i++)
			elem[i] = a.elem[i];

	}
	//	생성자까지 바디는 다 동일

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
		//	pos가 3번 먼저
		if (pos < elem || pos >= elem + sz)
			throw runtime_error("범위를 벗어났습니다.\n");

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

		cout << "\n턴이 변경되었습니다. 현재 턴: " << (turn == 0 ? player1.name : player2.name) << endl;
		cout << "이동할 위치를 다시 입력하세요(예 3454, 종료: q): ";
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
					cout << "킹을 잡았습니다. " << player1.name << "의 승리입니다!!" << endl;
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
				cout << "이동이 불가능한 위치입니다\n";
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
					cout << "킹을 잡았습니다. " << player2.name << "의 승리입니다!!" << endl;
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
				cout << "이동이 불가능한 위치입니다\n";
				return;
			}
		}
	}


	bool checkConditionWhite(char input1, char input2, char result1, char result2) {
		//	입력받은 위치의 기물이 해당하는 칸으로 이동할 수 있는지만 확인함ㄷㄷ

		
			turn = 1;
		

		if (result1 == input1 && result2 == input2) 
			//	같은 위치로 가는 경우는 false 리턴함
			return false;
		

		if (result1 >= 8 || result2 >= 8 || input1 >= 8 || input2 >= 8)
			//	목적지나 출발지가 보드를 벗어나는 경우 
			return false;

		if (board[input1][input2].first == PieceColor::WHITE) {
			if (board[input1][input2].second == PieceState::PAWN) {
				//	폰이 기본적으로 갈 수 있는 위 방향의 3칸의 경우에 대해서 확인함

				if (result1 == input1 - 1 && result2 == input2) {
					//	직진
					if (board[result1][result2].first == PieceColor::NONE) {
						return true;
					}
					else {
						return false;
					}
				}
				else if (result1 == input1 - 1 && result2 == input2 - 1) {
					//	위왼쪽
					if (board[result1][result2].first == PieceColor::BLACK)
						return true;
					else
						return false;
				}
				else if (result1 == input1 - 1 && result2 == input2 + 1) {
					//	위오른쪽
					if (board[result1][result2].first == PieceColor::BLACK)
						return true;
					else
						return false;
				}
				else
					return false;
			}

			else if (board[input1][input2].second == PieceState::ROOK) {
				//	룩은 상하좌우 직선으로 갈 수 있지만 상대편이나 다른 기물이 가로막으면 거기까지만 갈 수 있음
				//	정확하게는 가로막기 바로전 까지 이동할 수 있다고 표시하고 상대편 기물이 가로막는 경우는 한 칸 더 이동할 수 있게
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
					//	위오른쪽
					return true;
				else if (result1 == input1 - 2 && result2 == input2 - 1 && board[result1][result2].first != PieceColor::WHITE)
					//	위왼쪽
					return true;
				else if (result1 == input1 - 1 && result2 == input2 + 2 && board[result1][result2].first != PieceColor::WHITE)
					//	오른쪽위
					return true;
				else if (result1 == input1 + 1 && result2 == input2 + 2 && board[result1][result2].first != PieceColor::WHITE)
					//	오른쪽아래
					return true;
				else if (result1 == input1 + 2 && result2 == input2 + 1 && board[result1][result2].first != PieceColor::WHITE)
					//	아래오른쪽
					return true;
				else if (result1 == input1 + 2 && result2 == input2 - 1 && board[result1][result2].first != PieceColor::WHITE)
					//	아래왼쪽
					return true;
				else if (result1 == input1 - 1 && result2 == input2 - 2 && board[result1][result2].first != PieceColor::WHITE)
					//	왼쪽위
					return true;
				else if (result1 == input1 + 1 && result2 == input2 - 2 && board[result1][result2].first != PieceColor::WHITE)
					//	왼쪽아래
					return true;
				else
					return false;
			}
			else if (board[input1][input2].second == PieceState::BISHOP) {
				//	비숍도 룩과 마찬가지지만 대각선으로 움직여서 끝까지 갈 수 있는 곳의 행과 열 두가지 정보를 저장해야 함
				char upright1 = input1, upright2 = input2;
				char upleft1 = input1, upleft2 = input2;
				char downright1 = input1, downright2 = input2;
				char downleft1 = input1, downleft2 = input2;

				for (int i = 1; i < 9; i++) {
					//	경우의 수가 아니라 포문이라
					//	반복문 루프돌면서 배열 바깥으로 빠져나가지 않게 해야함
					//	위오른쪽 대각선
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
					//	위
					if (result1 >= up )
						return true;
					else
						return false;
				}
				if (result1 > input1 && result2 == input2) {
					//	아래
					if (result1 <= down)
						return true;
					else
						return false;
				}
				if (result1 == input1 && result2 < input2) {
					//	왼쪽
					if (result2 >= left)
						return true;
					else
						return false;
				}
				if (result1 == input1 && result2 > input2) {
					//	오른쪽
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
					//	직진
					if (board[result1][result2].first != PieceColor::WHITE) {
						return true;
					}
					else {
						return false;
					}
				}
				else if (result1 == input1 + 1 && result2 == input2) {
					//	뒤로
					if (board[result1][result2].first != PieceColor::WHITE)
						return true;
					else
						return false;
				}
				else if (result1 == input1 && result2 == input2 - 1) {
					//	왼족
					if (board[result1][result2].first != PieceColor::WHITE)
						return true;
					else
						return false;
				}
				else if (result1 == input1 && result2 == input2 + 1) {
					//	오른쪽
					if (board[result1][result2].first != PieceColor::WHITE)
						return true;
					else
						return false;
				}
				else if (result1 == input1 - 1 && result2 == input2 + 1) {
					//	위오른쪽
					if (board[result1][result2].first != PieceColor::WHITE)
						return true;
					else
						return false;
				}
				else if (result1 == input1 - 1 && result2 == input2 - 1) {
					//	위왼쪽
					if (board[result1][result2].first != PieceColor::WHITE)
						return true;
					else
						return false;
				}
				else if (result1 == input1 + 1 && result2 == input2 + 1) {
					//	아래오른쪽
					if (board[result1][result2].first != PieceColor::WHITE)
						return true;
					else
						return false;
				}
				else if (result1 == input1 + 1 && result2 == input2 - 1) {
					//	아래왼쪽
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
			cout << endl << "자신의 턴이 아닙니다" << endl;
			turn = 0;
			return false;
			}

		else {
			return false;
		}
	}

	bool checkConditionBlack(char input1, char input2, char result1, char result2) {
		//	입력받은 위치의 기물이 해당하는 칸으로 이동할 수 있는지만 확인함

		
			turn = 0;
		

		if (result1 == input1 && result2 == input2)
			//	같은 위치로 가는 경우는 false 리턴함
			return false;


		if (result1 >= 8 || result2 >= 8 || input1 >= 8 || input2 >= 8)
			//	목적지나 출발지가 보드를 벗어나는 경우 
			return false;

		if (board[input1][input2].first == PieceColor::BLACK) {
			if (board[input1][input2].second == PieceState::PAWN) {
				//	폰이 기본적으로 갈 수 있는 위 방향의 3칸의 경우에 대해서 확인함

				if (result1 == input1 + 1 && result2 == input2) {
					//	직진
					if (board[result1][result2].first == PieceColor::NONE) {
						return true;
					}
					else {
						return false;
					}
				}
				else if (result1 == input1 + 1 && result2 == input2 - 1) {
					//	아래왼쪽
					if (board[result1][result2].first == PieceColor::WHITE)
						return true;
					else
						return false;
				}
				else if (result1 == input1 + 1 && result2 == input2 + 1) {
					//	아래오른쪽
					if (board[result1][result2].first == PieceColor::WHITE)
						return true;
					else
						return false;
				}
				else
					return false;
			}

			else if (board[input1][input2].second == PieceState::ROOK) {
				//	룩은 상하좌우 직선으로 갈 수 있지만 상대편이나 다른 기물이 가로막으면 거기까지만 갈 수 있음
				//	정확하게는 가로막기 바로전 까지 이동할 수 있다고 표시하고 상대편 기물이 가로막는 경우는 한 칸 더 이동할 수 있게
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
					//	위오른쪽
					return true;
				else if (result1 == input1 - 2 && result2 == input2 - 1 && board[result1][result2].first != PieceColor::BLACK)
					//	위왼쪽
					return true;
				else if (result1 == input1 - 1 && result2 == input2 + 2 && board[result1][result2].first != PieceColor::BLACK)
					//	오른쪽위
					return true;
				else if (result1 == input1 + 1 && result2 == input2 + 2 && board[result1][result2].first != PieceColor::BLACK)
					//	오른쪽아래
					return true;
				else if (result1 == input1 + 2 && result2 == input2 + 1 && board[result1][result2].first != PieceColor::BLACK)
					//	아래오른쪽
					return true;
				else if (result1 == input1 + 2 && result2 == input2 - 1 && board[result1][result2].first != PieceColor::BLACK)
					//	아래왼쪽
					return true;
				else if (result1 == input1 - 1 && result2 == input2 - 2 && board[result1][result2].first != PieceColor::BLACK)
					//	왼쪽위
					return true;
				else if (result1 == input1 + 1 && result2 == input2 - 2 && board[result1][result2].first != PieceColor::BLACK)
					//	왼쪽아래
					return true;
				else
					return false;
			}
			else if (board[input1][input2].second == PieceState::BISHOP) {
				//	비숍도 룩과 마찬가지지만 대각선으로 움직여서 끝까지 갈 수 있는 곳의 행과 열 두가지 정보를 저장해야 함
				char upright1 = input1, upright2 = input2;
				char upleft1 = input1, upleft2 = input2;
				char downright1 = input1, downright2 = input2;
				char downleft1 = input1, downleft2 = input2;

				for (int i = 1; i < 9; i++) {
					//	경우의 수가 아니라 포문이라
					//	반복문 루프돌면서 배열 바깥으로 빠져나가지 않게 해야함
					//	위오른쪽 대각선
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
					//	위
					if (result1 >= up)
						return true;
					else
						return false;
				}
				if (result1 > input1 && result2 == input2) {
					//	아래
					if (result1 <= down)
						return true;
					else
						return false;
				}
				if (result1 == input1 && result2 < input2) {
					//	왼쪽
					if (result2 >= left)
						return true;
					else
						return false;
				}
				if (result1 == input1 && result2 > input2) {
					//	오른쪽
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
					//	직진
					if (board[result1][result2].first != PieceColor::BLACK) {
						return true;
					}
					else {
						return false;
					}
				}
				else if (result1 == input1 + 1 && result2 == input2) {
					//	뒤로
					if (board[result1][result2].first != PieceColor::BLACK)
						return true;
					else
						return false;
				}
				else if (result1 == input1 && result2 == input2 - 1) {
					//	왼족
					if (board[result1][result2].first != PieceColor::BLACK)
						return true;
					else
						return false;
				}
				else if (result1 == input1 && result2 == input2 + 1) {
					//	오른쪽
					if (board[result1][result2].first != PieceColor::BLACK)
						return true;
					else
						return false;
				}
				else if (result1 == input1 - 1 && result2 == input2 + 1) {
					//	위오른쪽
					if (board[result1][result2].first != PieceColor::WHITE)
						return true;
					else
						return false;
				}
				else if (result1 == input1 - 1 && result2 == input2 - 1) {
					//	위왼쪽
					if (board[result1][result2].first != PieceColor::WHITE)
						return true;
					else
						return false;
				}
				else if (result1 == input1 + 1 && result2 == input2 + 1) {
					//	아래오른쪽
					if (board[result1][result2].first != PieceColor::WHITE)
						return true;
					else
						return false;
				}
				else if (result1 == input1 + 1 && result2 == input2 - 1) {
					//	아래왼쪽
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
			cout << "자신의 턴이 아닙니다" << endl;
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

	cout << "플레이어 두명의 이름을 입력하세요: ";
	cin >> player1.name >> player2.name;

	thread thread1(setTimer, ref(chess1), ref(player1), ref(player2));

	chess1.initialize();
	chess1.printBoard();

	while (1) {
		int i = 0;
		char op[4] = "";

		cout << endl;
		cout << "원하는 기물의 현재위치와 이동할 위치를 입력하세요(예 3454, 종료: q): ";

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

		cout << "현재 점수는 " << player1.name << "님이 " << player1.point << ", " << player2.name << "님이 " << player2.point << "를 흭득한 상태입니다." << endl;

		traceTime = 0;

		cout << endl << endl << endl;
		chess1.printBoard();

		
	}

		working = false;

		if (thread1.joinable()) {
			thread1.join();
		}

		cout << "게임이 종료되었습니다." << endl;

		return 0;
	}//	1