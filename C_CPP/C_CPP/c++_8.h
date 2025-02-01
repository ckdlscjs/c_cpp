#pragma once
#include "std.h"
namespace ExcelProject
{
	class Cell;
	class Table;
	class Vector
	{
		int capacity;
		int length;
		std::string* datas;
	public:
		// 생성자
		Vector(int n = 1) : capacity(n), length(0), datas(new std::string[n]) {}
		virtual ~Vector()
		{
			if(datas)
				delete[] datas;
		}
		// 맨 뒤에 새로운 원소를 추가한다.
		void push_back(std::string s)
		{
			if (length >= capacity)
			{
				std::string* copys = new std::string[capacity * 2];
				for (int idx = 0; idx < length; idx++) copys[idx] = datas[idx];
				delete[] datas;
				datas = copys;
				capacity *= 2;
			}
			datas[length++] = s;
		}

		// 임의의 위치의 원소에 접근한다.
		std::string operator[](int i)
		{
			if (i < 0 || i >= length) return "";
			return datas[i];
		}

		// x 번째 위치한 원소를 제거한다.
		void remove(int x)
		{
			if (x < 0 || x >= length) return;
			for (int i = x + 1; i < length; i++)
			{
				datas[i - 1] = datas[i];
			}
			length--;
		}

		// 현재 벡터의 크기를 구한다.
		int size()
		{
			return length;
		}
	};
	class Stack
	{
		struct Node
		{
			std::string str;
			Node* prev;
			Node(std::string _s, Node* _prev = NULL) : str(_s), prev(_prev){}
		};
		Node* topNode;
	public:
		Stack() : topNode(NULL) {}
		virtual ~Stack()
		{
			while (topNode)
			{
				Node* delNode = topNode;
				topNode = delNode->prev;
				delete delNode;
			}
		}
		// 최상단에 새로운 원소를 추가한다.
		void push(std::string str)
		{
			Node* newNode = new Node(str);
			if (topNode == NULL)
			{
				topNode = newNode;
			}
			else
			{
				newNode->prev = topNode;
				topNode = newNode;
			}
		}
		// 최상단의 원소를 반환한다.
		std::string top()
		{
			return topNode ? topNode->str : NULL;
		}

		// 최상단의 원소를 제거하고 반환한다.
		std::string pop()
		{
			Node* delNode = topNode;
			std::string ret = delNode->str;
			topNode = delNode->prev;
			delete delNode;
			return ret;
		}

		// 스택이 비어있는지의 유무를 반환한다.
		bool is_empty()
		{
			return topNode ? false : true;
		}
	};
	class Cell
	{
	public:
		std::string data;
		int y, x;
		Table* table;
	public:
		Cell(std::string _str, int _y, int _x, Table* _tableinfo) : data(_str), y(_y), x(_x), table(_tableinfo) {}
		virtual std::string numtostr()
		{
			return data;
		}
		virtual int strtonum()
		{
			return 0;
		}
	};

	/*
	* cell* ptr = new cell->cell을 가리키는 포인터
	* cell* ptrs = new cell[size] -> size만큼 동적생성한 cell배열의 시작주소를 가리키는 포인터
	* cell** dptrs = new cell*[size] -> cell*을 가리키는 포인터를 size갯수만큼 동적할당하고 그 배열의 시작주소를 할당 ....
	*/

	class Table
	{
	protected:
		int size_row, size_col;
		Cell*** cells;
	public:
		Table(int _rsize, int _csize) : size_row(_rsize), size_col(_csize)
		{
			cells = new Cell**[size_row]; //cells**형 포인터를 size개수만큼 가지는 배열의 시작주소를 cells에 할당, row행만큼
			for (int r = 0; r < size_row; r++)
			{
				cells[r] = new Cell * [size_col]; //cells* 포인터를 size개수만큼 가지는 배열의 시작주소를 cells[r](행) 에 할당
				//std::memset(cells[r], NULL, sizeof(Cell*) * size_col);
				for (int c = 0; c < size_col; c++)
				{
					cells[r][c] = NULL;
				}
			}
		}
		~Table()
		{
			//원소(cell을 가리키는 동적할당) 먼저 제거
			for (int r = 0; r < size_row; r++)
				for (int c = 0; c < size_col; c++)
					if (cells[r][c]) delete cells[r][c];
			//행(cell*을 가리키는 r개의 cell**형 포인터, cell**은 size_col개를 들고있음)
			for (int r = 0; r < size_row; r++)
				delete[] cells[r];
			//전체(cell**을 가리키는 cell***형 포인터 cells, size_row개를 들고있음)
			delete[] cells;
		}
		// 새로운 셀을 row 행 col 열에 등록한다.
		void reg_cell(Cell* cell)
		{
			if (!cell) return;
			int r = cell->y;
			int c = cell->x;
			if (!(0 <= r && r < size_row)) return;
			if (!(0 <= c && c < size_col)) return;
			if (cells[r][c]) delete cells[r][c];
			cells[r][c] = cell;
		}

		// 해당 셀의 정수값을 반환한다.
		// s : 셀 이름 (Ex. A3, B6 과 같이)
		int to_numeric(const std::string& s)
		{
			int r = std::atoi(s.c_str()+1);
			int c = s[0] - 'A';
			return to_numeric(r, c);
		}

		// 행 및 열 번호로 셀을 호출한다.
		int to_numeric(int r, int c)
		{
			if (!(0 <= r && r < size_row)) return -1;
			if (!(0 <= c && c < size_col)) return -1;
			if (!cells[r][c]) return -1;
			return cells[r][c]->strtonum();
		}

		// 해당 셀의 문자열을 반환한다.
		std::string stringify(const std::string& s)
		{
			int r = std::atoi(s.c_str() + 1);
			int c = s[0] - 'A';
			return stringify(r, c);
		}
		std::string stringify(int r, int c)
		{
			if (!(0 <= r && r < size_row)) return "";
			if (!(0 <= c && c < size_col)) return "";
			if (!cells[r][c]) return "";
			return cells[r][c]->numtostr();
		}

		virtual std::string print_table() const = 0; //const로 함수선언해야 전역함수 operator에서 const로 호출 가능, 순수가상함수, 자식클래스에서 오버라이드해야함
	};
	class TextTable : public Table
	{
	private:
		std::string repeat_char(int n, char c) const
		{
			std::string str = "";
			for (int i = 0; i < n; i++) str += c;
			return str;
		}
		// 숫자로 된 열 번호를 A, B, .... Z, AA, AB, ...  이런 순으로 매겨준다.
		std::string col_num_to_str(int n) const
		{
			std::string str = "";
			if (n < 26)
			{
				str += ('A' + n);
			}
			else
			{
				char first = 'A' + n / 26 - 1;
				char second = 'A' + n % 26;

				str.push_back(first);
				str.push_back(second);
			}
			return str;
		}
	public:
		//부모생성자에 변수전달 및 생성
		TextTable(int row, int col) : Table(row, col) {}

		// 텍스트로 표를 깨끗하게 출력해준다.
		virtual std::string print_table() const
		{
			//열의 길이를 맞추기위한 열의길이정보조회
			int* col_max_lengths = new int[size_col];
			for (int j = 0; j < size_col; j++)
			{
				unsigned int col_length = 2;
				for (int i = 0; i < size_row; i++)
				{
					if (cells[i][j] && cells[i][j]->numtostr().size() > col_length)
						col_length = cells[i][j]->numtostr().size();
				}
				col_max_lengths[j] = col_length;
			}
			
			//맨윗줄(열정보)
			int table_width = 4;
			std::string total_table = "    "; //4칸
			for (int j = 0; j < size_col; j++)
			{
				if (col_max_lengths[j])
				{
					int length = std::max(2, col_max_lengths[j]);
					total_table += " | " + col_num_to_str(j);
					total_table += repeat_char(length - col_num_to_str(j).size(), ' ');
					table_width += (length + 3);
				}
			}
			total_table += "\n";

			//행, 열 데이터표시
			for (int i = 0; i < size_row; i++) {
				total_table += repeat_char(table_width, '-'); //공백+길이만큼 - 표시
				total_table += "\n"; //줄개행
				total_table += to_string(i + 1) + repeat_char(4 - to_string(i + 1).length(), ' '); //행 표시

				for (int j = 0; j < size_col; j++) {
					if (col_max_lengths[j]) {
						int max_len = std::max(2, col_max_lengths[j]); //특정열길이

						string s = "";
						if (cells[i][j]) {
							s = cells[i][j]->numtostr(); //내용표시
						}
						total_table += " | " + s;
						total_table += repeat_char(max_len - s.size(), ' '); //내용표시및 공백 줄맞춤
					}
				}
				total_table += "\n"; //한 행의 출력 끝내고 개행
			}

			return total_table;
		}
	};
	std::ostream& operator<<(std::ostream& out, const Table& table)
	{
		out << table.print_table();
		return out;
	}
	void Exam()
	{
		TextTable table(5, 6);
		std::ofstream out("test.txt");

		table.reg_cell(new Cell("Hello~", 0, 0, &table));
		table.reg_cell(new Cell("C++", 0, 1, &table));

		table.reg_cell(new Cell("Programming", 1, 1, &table));
		std::cout << std::endl << table;
		out << table;
	}
}