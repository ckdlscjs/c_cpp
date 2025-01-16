#pragma once
#include "std.h"
/*
* c++의 입출력 클래스들은 ios_base를 기반클래스로 한다, ios_base는 스트림입출력형식관련 데이터를 처리한다(실수형, 정수형, 10진수, 16진수 자료형데이터처리)
* 
* ios클래스는 iosbase를 기반으로 하여 스트림버퍼를 초기화한다, 데이터를 내보내거나 받아들이기 전에 임시로 저장하는곳, 데이터를 한번에 읽어 스트림버퍼에 저장후
* 필요시에 따라 원하는 크기만큼 꺼낸다, 데이터를 읽어오는 작업이 매우 느리기 때문에 미리 청크단위로 읽어와 필요한만큼 사용한다
* 입출력작업의 상태처리또한 처리한다, 파일을 읽다가 끝에 도달했는가를 확인하기위해 eof함수를 호출, 입출력작업의 성공여부를 체크하는 good함수또한 처리
* 
* istream 은 입력을 수행하는 클래스이다, istream객체 cin에대한 오버로딩 연산자 operator>>을 이용해서 입력을 받는것
* std::string클래스의 객체또한 xstring에서 istream의 전역함수를 string객체에 대해 오버로딩해서 정의하였기때문
* operator>>는 공백문자(엔터, 띄어쓰기, 탭)을 받을 수 없다, 마무리문자로 취급된다
* ios클래스는 스트림의 상태를 관리하는데 이때 4개의 플래그를 이용해 현재 스트림이 어떠한 상태인지 정보를 관리한다
* goodbit -> 스트림에 입출력 작업이 가능함
* badbit -> 스트림에 복구 불가능한 오류가 발생
* failbit -> 스트림에 복구 가능한 오류가 발생
* eofbit -> 입력작업이 eof도달(끝)
* 
* std::cin의 operator>>를 이용해서 입력을 받을때 스트림의 상태에 따라서 객체내부의 플래그를 이용해 상태를 변경시키는데
* 올바르지 않은 입력값이 들어온다면 operator함수는 cin객체의 failbit등을 키게된다, 이후 cin객체의 상태에따른 리턴값(cin객체)의 상태에 따라
* 추후입력등을 받을수 있는가 없는가에대한 여부가 정해진다
* 
* std::cin.setf(std::ios_base::hex, std::ios_base::basefield); 를 이용해서 스트림의 입출력 형식을 16진수로(std::ios_base::hex)로 바꿀수 있다
* setf의 함수인자가 위처럼 2개일경우 첫번째 플래그 인자를 적용시키고 두번째 인자의 내용을 초기화시키는것
* 
* 두번째 입출력변경방법으로는 std::cin >> std::hex 를 통하여 변경 할 수 있다, 위의 setf의경우 인자로 플래그를(std::ios_base::hex)지정하는 것 이지만
* std::cin >> std::hex의 경우 std::ios_base를 리턴하는 std::ios_base& hex(std::ios_base& str) 함수를 이용하여 인자로 들어온 cin(istream)의 값을 리턴하여 재대입 하는것으로 변경한다
* 고로 std::cin 을 기준으로 operator>>로 통해 ios_base로 리턴되는 std::hex함수를 이용하여 operator>>의 인자로 리턴되는 ios_base를 받아 cin의 플래그값을 변경시키는것
* 이때 함수의 인자는 istream& operator>>(ios_base& (*pf)(ios_base&))가 되는데 이때 인자인 ios_base& (*pf)(ios_base&)은 함수포인터를 의미한다 고로 std::hex가 함수이기 때문이다
* 위의 두가지 방법처럼 특정 플래그를 이용해 스트림의상태를 바꾸는것들을 조작자 라고 한다(endl, left, right, boolalpha등)
* 
* 모든 입출력 객체들은 대응되는 스트림객체를 가진다, 입출력에 해당하는 스트림버퍼 클래스는 streambuf클래스이다.
* 문자들의 순차적인 나열을 stream이라고도 볼 수 있는데 c++에서는 std::stringstream 을 통해 문자열을 스트림처럼 이용 할 수도 있게 한다
* 
* 
*/
namespace stream
{
	void Exam()
	{
		/*
		int t = -1;
		while (true) {
			std::cin >> t;
			if (std::cin.fail()) //cin객체의 MyState, flag와 &연산하여 결과를 리턴
			{
				std::cout << "제대로입력바람"<< '\n';
				std::cin.clear();
				std::cin.ignore(100, '\n');
			}
			else
			{
				std::cout << "입력 :: " << t << '\n';
			}
			
			if (t == 0) 
				break;
		}
		*/

		/*
		char first, last;

		cout << "Enter your first and last names: ";

		first = cin.get();
		cin.ignore(256, ' ');  // 스트림에서 256개, ' '까지 무시한다

		last = cin.get();

		cout << "Your initials are " << first << last;
		*/

		std::string s;
		std::cin >> s;

		// 위치 지정자를 한 칸 옮기고, 그 다음 문자를 훔쳐본다 (이 때는 움직이지 않음)
		/*
		* std::cin >> s 를 이용해 hello world\n를 입력받으면
		* 띄어쓰기를 만난 스트림버퍼는 o와 w의 가운데 공백을 지정하고있다
		* rdbuf를 이용해 현재 스트림에대한 포인터를 가져오고 snextc는 스트림의 위치지정자를 한칸 뒤로 전진후 엿본다, 새로운 입력이 들어올때 까지는
		* 위치를 읽는것이 아니기때문에 가리키는 지점만 w를 지정하게되고 이후 std::cin>>s로 새입력(world\n)가 들어오게 되면 그때 w를 읽어온다
		*/
		char peek = std::cin.rdbuf()->snextc();
		if (std::cin.fail()) std::cout << "Failed";
		std::cout << "두 번째 단어 맨 앞글자 : " << peek << std::endl;
		//std::cin >> s;
		//std::cout << "다시 읽으면 : " << s << std::endl;
	}
}
namespace fileio
{
//#define __str__
	void Exam()
	{
		std::ifstream in("test.txt"); //파일명만 있을시 vs에서 실행시 소스파일과같은경로, 이외엔 실행파일과 같은경로, 객체가 새로운 파일을 읽기위해선 close가 필요하다, ifstream객체는 소멸시 자동으로 close를한다
		std::string s;
		if (in.is_open())
		{
			in >> s;
			std::cout << s << '\n';
		}
		else
			std::cout << "파일을 못읽어옴\n";
	}
	void Exam2()
	{
		std::ifstream in("test2.txt", std::ios::binary); //입력을 어떤형태로 받을가에대한 플래그, or연산으로 추가 옵션가능
		std::string s;
		int x;
		if (in.is_open())
		{
			in.read((char*)(&x), 4);
			std::cout << std::hex << x << '\n';
		}
		else
		{
			std::cout << "파일을 찾을 수 없습니다\n";
		}
	}
	void Exam3()
	{
		std::ifstream in("test3.txt");
		std::string s;
		if (in.is_open())
		{
			//파일의마지막으로 포인터이동
			in.seekg(0, std::ios::end);
			//현재위치를 기준으로 크기를 리턴, 한글은 2바이트씩임
			int size = in.tellg();
			//문자열의 크기를 사이즈만큼지정
			s.resize(size);
			//포인터를처음으로리턴
			in.seekg(0, std::ios::beg);
			//파일의 처음부터 사이즈만큼 지정해읽는다
			in.read(&s[0], size);
			std::cout << s << '\n';//출력
		}
		else
		{
			std::cout << "파일을 찾을 수 없습니다\n";
		}
	}
	void Exam4()
	{
		std::ifstream in("test3.txt");
		char buf[105];
		if (!in.is_open())
		{
			std::cout << "파일을 읽을 수 없습니다\n";
			return;
		}

#ifdef __str__
		while (in) //자신을 bool로 캐스팅 하는 operator bool()이 오버로딩 되어있어 bool로 캐스팅 하는 operator bool이 호출되어 조건체크에 쓰인다, 오류플래그를 기준으로 리턴boolean값이정해짐
		{ 
			in.getline(buf, 100); //'\n'을 만날때 까지 100개 단위로 읽어온다, 마지막'\n'을 제외한 최대 99글자
			std::cout << buf << std::endl;
			/*
			* getline함수는 개행문자 혹은 지정문자(세번째 패러미터 delim)가 나오기 전에 버퍼의 크기가 다 차면(사이즈를 넘기면) failbit을 켜므로 사이즈조건을 꼭 체크해야한다
			* 이를 해결하기위해 ifstream의 getline이 아닌 std::getline(ifstream객체, std::string)을 이용하여 받으면 사이즈를 신경쓰지 않아도 된다
			*/
		}
#endif
#ifndef  __str__
		std::string str;
		while (in) //자신을 bool로 캐스팅 하는 operator bool()이 오버로딩 되어있어 bool로 캐스팅 하는 operator bool이 호출되어 조건체크에 쓰인다, 오류플래그를 기준으로 리턴boolean값이정해짐
		{
			std::getline(in, str);
			std::cout << str << std::endl;

		}
#endif // ! __str__
	}

	void Exam5()
	{
		/*std::ofstream out("testout.txt"); //파일이 존재시 열고 지운후 기록, 없으면 생성후 기록
		if (out.is_open())
		{
			out << "WriteThis!";
		}
		*/
		
		std::ofstream out("testout.txt", std::ios::app); //std::ios::app을 옵션으로 추가시 append이어붙힌다.
		if (out.is_open())
		{
			out << "appendThis!";
		}
		/*
		* ios::binary, app말고도 여러가지 옵션이 존재한다
		* ios::ate -> 자동으로 파일 끝에서 부터 읽기, 쓰기를 실시한다(파일을 열고 위치지정자가 파일 끝을 가리키고있다), ate의경우 위치지정자를 앞으로 옮길수도있다, app는 무조건 맨끝
		* ios::trunc -> 파일 스트림을 열면 기존 내용이 모두지워진다, 기본적인 ofstream객체 생성시의 초기옵션
		* ios::in, ios::out -> 파일에 입력/ 혹은 출력 여부를 결정한다, ifstream, ofstream시의 기본옵션들
		*/
	}
	class EX6Class
	{
	private:
		std::string name;
		int age;
	public:
		EX6Class(const std::string _name, const int _age) : name(_name), age(_age) {}
		friend ofstream& operator<<(std::ofstream& out, EX6Class& ex) //전역함수를 오버로딩
		{
			out << ex.name + ", " + std::to_string(ex.age) << '\n';
			return out;
		}
	};
	void Exam6()
	{
		std::ofstream out("test6.txt");
		EX6Class ex("이재범", 60);
		out << ex;
	}
}
