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

	class NumStack
	{
		struct Node
		{
			double num;
			Node* prev;
			Node(double _num, Node* _prev = NULL) : num(_num), prev(_prev) {}
		};
		Node* topNode;
	public:
		NumStack() : topNode(NULL) {}
		virtual ~NumStack()
		{
			while (topNode)
			{
				Node* delNode = topNode;
				topNode = delNode->prev;
				delete delNode;
			}
		}
		// 최상단에 새로운 원소를 추가한다.
		void push(double num)
		{
			Node* newNode = new Node(num);
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
		double top()
		{
			return topNode ? topNode->num : NULL;
		}

		// 최상단의 원소를 제거하고 반환한다.
		double pop()
		{
			Node* delNode = topNode;
			double ret = delNode->num;
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
	/*
	* 8_1파트에서는 체크위해 cell자체가동작되게끔구현
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
	*/
	//8-2, 추상클래스로 놔두고 자식클래스에서 상속받아 구현, 인터페이스와의 차이점은 다중상속이 필요한 기능구현만 필요할경우 인터페이스로 모듈화, 공통분모는 추상클래스로
	class Cell
	{
	protected:
		int y, x;
		Table* table;
	public:
		Cell(int _y, int _x, Table* _tableinfo) :y(_y), x(_x), table(_tableinfo) {}
		virtual std::string numtostr() = 0;
		virtual int strtonum() = 0;
		int Y() { return y; }
		int X() { return x; }
	};

	class StringCell : public Cell
	{
	private:
		std::string data;
	public:
		StringCell(std::string _str, int _y, int _x, Table* _tableinfo) : Cell(_y, _x, _tableinfo), data(_str)
		{

		}
		virtual std::string numtostr() override
		{
			return data;
		}
		virtual int strtonum() override
		{
			return 0;
		}
	};

	class NumberCell : public Cell
	{
	private:
		int data;
	public:
		NumberCell(int _num, int _y, int _x, Table* _tableinfo) : Cell(_y, _x, _tableinfo), data(_num)
		{

		}
		virtual std::string numtostr() override
		{
			return std::to_string(data);
		}
		virtual int strtonum() override
		{
			return data;
		}
	};

	class DateCell : public Cell
	{
	private:
		//1970년 부터 현재 시간까지 몇초가 흘렀는지 보관하는 정수형 변수
		std::time_t data;
	public:
		DateCell(std::string _str, int _y, int _x, Table* _tableinfo) : Cell(_y, _x, _tableinfo)
		{
			//입력받는 날짜형식 문자열은 yyyy-mm-dd로 받는다
			int year = std::atoi(_str.c_str()); //atoi는 숫자 까지만 변환해주므로 -에서 변환을멈춘다, 0~3까지변환
			int month = std::atoi(_str.c_str() + 5);//5~6까지변환
			int day = std::atoi(_str.c_str() + 8);//8~9까지 변환
			std::tm timeinfo; //std 시간 구조체 tm
			timeinfo.tm_year = year - 1900;
			timeinfo.tm_mon = month - 1;
			timeinfo.tm_mday = day;
			timeinfo.tm_hour = 0;
			timeinfo.tm_min = 0;
			timeinfo.tm_sec = 0;
			data = std::mktime(&timeinfo); //int64형으로 구조체를 기반으로 리턴하는 함수
		}
		virtual std::string numtostr() override
		{
			char buf[50];
			std::tm temp;
			localtime_s(&temp, &data);
			strftime(buf, 50, "%F", &temp);
			return std::string(buf);
		}
		virtual int strtonum() override
		{
			return static_cast<int>(data);
		}
	};
	class ExpressionCell : public Cell
	{
	private:
		std::string data;
		//string* parsed_expr; //notuse

		Vector exp_vec;

		//1.피연산자는 그냥 exp_vec에집어넣는다
		//2.여는괄호를 만날경우 스택에push
		//3.닫는괄호를 만날경우 기존에 넣었던 여는괄호가 pop될 때 까지 pop되는 연산자들을 exp_vec에 넣는다
		//4.연산자일 경우 자기보다 우선순위가 낮은 연산자가 스택의 최상단, 혹은 스택이 빌 때 까지 스택을 pop하고 pop된연산자들을 exp_vec에 넣는다, 마지막에 자기자신을 스택에push
		// 연산자 우선 순위를 반환합니다
		int precedence(char c)
		{
			switch (c)
			{
			case '(':
			case '[':
			case '{':
				return 0;

			}
		}

		// 수식을 분석합니다.
		void parse_expression()
		{

		}
	public:
		ExpressionCell(int _num, int _y, int _x, Table* _tableinfo) : Cell(_y, _x, _tableinfo), data(_num)
		{

		}
		virtual std::string numtostr() override
		{
			
		}
		//후위표기식을 바탕으로 수식을 계산해 반환
		virtual int strtonum() override
		{
			double result = 0;
			
		}
	};

	

	class Table
	{
	/*
	* cell* ptr = new cell->cell을 가리키는 포인터
	* cell* ptrs = new cell[size] -> size만큼 동적생성한 cell배열의 시작주소를 가리키는 포인터
	* cell** dptrs = new cell*[size] -> cell*을 가리키는 포인터를 size갯수만큼 동적할당하고 그 배열의 시작주소를 할당 ....
	*/
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
			int r = cell->Y();
			int c = cell->X();
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

		table.reg_cell(new StringCell("Hello~", 0, 0, &table));
		table.reg_cell(new StringCell("C++", 0, 1, &table));

		table.reg_cell(new StringCell("Programming", 1, 1, &table));
		std::cout << std::endl << table;
		out << table;
	}
}