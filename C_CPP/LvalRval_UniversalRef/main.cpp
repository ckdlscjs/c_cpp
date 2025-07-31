#include "bits/stdc++.h"
/*
1. lvalue(엘밸류) 와 rvalue(알밸류)
C++에서 모든 표현식(expression)은 크게 두 가지 범주 중 하나에 속합니다 : lvalue 또는 rvalue.이 둘은 "할당 연산자(=)의 왼쪽에 올 수 있는가?"라는 기준으로 생각하면 이해하기 쉽습니다.

lvalue(locator value) :

    메모리 주소를 가지며, 이름이 있는(named) 객체를 나타냅니다.

    할당 연산자의 왼쪽에 올 수 있습니다.즉, 값을 변경할 수 있습니다.

    예시 :

    C++

    int x = 10; // x는 lvalue (메모리 주소를 가지고, 값을 바꿀 수 있음)
x = 20;     // OK

std::vector<int> myVec; // myVec은 lvalue
myVec.push_back(1);
변수 이름, 함수 반환값 중 참조(reference) 타입, 배열 요소 등이 lvalue에 해당합니다.

rvalue(right value) :

    메모리 주소를 가지지 않는 임시적인 값을 나타냅니다.

    할당 연산자의 왼쪽에 올 수 없습니다.즉, 값을 변경할 수 없습니다.수명이 짧습니다.

    예시 :

    C++

    int result = 10 + 20; // 10, 20, 그리고 (10 + 20)의 결과인 30은 모두 rvalue (임시 값)
// 30 = 40; // X - 불가능 (30은 임시 값이라 할당 연산자 왼쪽에 올 수 없음)

int y = 5;
result = y + 7; // (y + 7)의 결과인 12는 rvalue

MyClass createObject(); // createObject() 함수의 반환값(임시 객체)은 rvalue
리터럴(literal) 상수, 임시 객체, 함수 반환값 중 참조가 아닌 비참조 타입 등이 rvalue에 해당합니다.
*/

//2. 이동 시맨틱(Move Semantics) 등장 배경
//이제 lvalue와 rvalue를 이해했으니, 왜 이동 시맨틱이 필요한지 알아볼까요 ?
//
//문제점 : 비효율적인 복사(Expensive Copy)
//
//std::vector나 std::string처럼 내부에 동적으로 할당된 메모리(힙 메모리)를 관리하는 객체를 생각해 봅시다.
//
//C++

std::vector<int> createBigVector() {
    std::vector<int> tempVec(1000000); // 100만 개의 int를 가진 벡터 생성
    // ... 데이터 채우기 ...
    return tempVec; // 1. tempVec은 함수 스코프를 벗어나며 임시 객체가 됨
}

int main() {
    std::vector<int> myVector = createBigVector(); // 2. 여기서 '복사 생성자'가 호출됩니다!
    // tempVec (임시 객체)의 100만 개 데이터를 myVector로 '깊은 복사'
    // 3. tempVec (임시 객체)은 소멸되며 힙 메모리를 해제합니다.
    return 0;
}
/*
위 코드에서 createBigVector()가 반환하는 tempVec은 rvalue입니다.myVector = createBigVector() 줄에서 복사 생성자가 호출되는데, 이 과정은 다음과 같습니다 :

tempVec이 관리하던 100만 개 int 데이터를 myVector를 위해 새로운 힙 메모리에 할당하고,

tempVec의 데이터를 새로운 메모리로 일일이 복사합니다.

이후 tempVec은 소멸되며 자신이 가지고 있던 힙 메모리를 해제합니다.

이 경우 tempVec은 어차피 사라질 임시 객체인데, 굳이 그 안의 데이터를 '깊은 복사'하고 나중에 해제하는 과정은 불필요한 자원 낭비(시간, 메모리)입니다.마치 "어차피 버릴 물건인데, 내용물만 새 포장지에 똑같이 옮겨 담고 원래 물건을 버리는" 격이죠.

해결책: 복사 대신 '이동'

여기서 이동 시맨틱의 아이디어가 나옵니다. "어차피 버려질 임시 객체라면, 그 객체가 관리하던 자원(힙 메모리 포인터 등)을 복사하지 말고 그냥 '뺏어오자'(move)!"

이것을 가능하게 하는 것이 rvalue reference와 이동 생성자 / 이동 대입 연산자입니다.
*/

/*
3. rvalue reference(우측값 참조&&)
rvalue reference는 rvalue에만 바인딩될 수 있는 참조 타입입니다.문법은 T && 입니다.

T & (lvalue reference) : lvalue에만 바인딩될 수 있습니다.rvalue에는 바인딩되지 않습니다.

C++

int x = 10;
int& ref = x; // OK, x는 lvalue
// int& ref2 = 10; // Error! 10은 rvalue
T && (rvalue reference) : rvalue에만 바인딩될 수 있습니다.lvalue에는 바인딩되지 않습니다.

C++

int x = 10;
// int&& rref = x; // Error! x는 lvalue
int&& rref2 = 10; // OK, 10은 rvalue

std::vector<int>&& vec_rref = createBigVector(); // OK, createBigVector()의 반환값은 rvalue
rvalue reference는 컴파일러에게 "이 참조는 임시 객체에 대한 참조이니, 그 자원을 마음껏 훔쳐도 안전하다"는 신호를 줍니다.
*/
/*
4. 이동 생성자(Move Constructor)
이동 생성자는 rvalue reference를 인자로 받는 특수한 형태의 생성자입니다.

목적: 임시 객체가 관리하던 자원을 효율적으로 "훔쳐와서" 새로운 객체에 연결하고, 원본 임시 객체는 자원을 가지지 않는 상태로 만들어 불필요한 복사와 해제를 방지합니다.

시그니처 : ClassName(ClassName&& other);

동작 방식 :

other가 관리하던 자원(예 : 힙 메모리 포인터)을 자신의 것으로 가져옵니다.

other의 자원 포인터를 nullptr 또는 유효하지 않은 값으로 설정하여 other가 소멸될 때 자원을 해제하지 않도록 합니다.

예시 : 간단한 MyVector 클래스

C++
*/
#include <iostream>
#include <utility> // std::move를 위해

class MyVector {
public:
    int* data;
    size_t size;

    // 생성자
    MyVector(size_t s) : size(s) {
        data = new int[size];
        std::cout << "MyVector(" << size << ") 생성자 호출, 메모리 할당: " << data << std::endl;
    }

    // 소멸자
    ~MyVector() {
        if (data) { // data가 nullptr이 아닌 경우에만 해제
            delete[] data;
            std::cout << "MyVector(" << size << ") 소멸자 호출, 메모리 해제: " << data << std::endl;
        }
        else {
            std::cout << "MyVector(" << size << ") 소멸자 호출, 이미 이동된 객체: " << data << std::endl;
        }
        data = nullptr; // 안전하게 nullptr로 설정
    }

    // 복사 생성자 (깊은 복사)
    MyVector(const MyVector& other) : size(other.size) {
        data = new int[size];
        std::copy(other.data, other.data + size, data);
        std::cout << "MyVector(" << size << ") 복사 생성자 호출, 새 메모리 할당 및 복사: " << data << std::endl;
    }

    // ⭐️ 이동 생성자 (자원 훔치기)
    MyVector(MyVector&& other) noexcept : data(other.data), size(other.size) {
        other.data = nullptr; // 원본 객체의 포인터를 nullptr로 만들어 소멸자가 해제하지 않도록 함
        other.size = 0;       // (선택 사항) 원본 객체의 상태를 초기화
        std::cout << "MyVector(" << size << ") 이동 생성자 호출, 자원 이동: " << data << std::endl;
    }

    // 복사 대입 연산자
    MyVector& operator=(const MyVector& other) {
        if (this != &other) {
            delete[] data; // 기존 자원 해제
            size = other.size;
            data = new int[size];
            std::copy(other.data, other.data + size, data);
            std::cout << "MyVector(" << size << ") 복사 대입 연산자 호출" << std::endl;
        }
        return *this;
    }

    // ⭐️ 이동 대입 연산자 (자원 훔치기)
    MyVector& operator=(MyVector&& other) noexcept {
        if (this != &other) {
            delete[] data; // 기존 자원 해제
            data = other.data; // 자원 훔치기
            size = other.size;
            other.data = nullptr; // 원본 객체의 포인터를 nullptr로
            other.size = 0;
            std::cout << "MyVector(" << size << ") 이동 대입 연산자 호출" << std::endl;
        }
        return *this;
    }
};

MyVector createMyVector(size_t s) {
    return MyVector(s); // 여기서 임시 객체가 생성되어 반환됨
}

int main() {
    std::cout << "--- 1. 복사 생성자 예시 ---" << std::endl;
    MyVector v1 = MyVector(5); // 생성자 호출
    MyVector v2 = v1;          // 복사 생성자 호출 (v1은 lvalue)
    std::cout << std::endl;

    std::cout << "--- 2. 이동 생성자 예시 ---" << std::endl;
    MyVector v3 = createMyVector(10); // createMyVector의 반환값은 rvalue이므로 이동 생성자 호출
    // 또는 MyVector v3 = MyVector(10); // 직접 rvalue를 넘길 때도 이동 생성자 호출
    std::cout << std::endl;

    std::cout << "--- 3. std::move 예시 ---" << std::endl;
    MyVector v4(20);
    MyVector v5 = std::move(v4); // v4는 lvalue이지만, std::move로 rvalue reference로 캐스팅되어 이동 생성자 호출
    // v4는 이제 '이동된 상태'이므로 더 이상 사용하지 않는 것이 안전합니다.
    std::cout << "v4의 data 포인터: " << v4.data << std::endl; // nullptr일 것임
    std::cout << std::endl;

    std::cout << "--- main 함수 끝 ---" << std::endl;
    return 0;
}
/*
위 예시를 실행해보면, 복사 생성자일 때는 새 메모리 할당 및 복사 메시지가 뜨고, 이동 생성자일 때는 자원 이동 메시지가 뜨면서 delete[] data가 한 번만 호출되는 것을 볼 수 있습니다.
*/
/*
5. std::move
std::move는 이름과는 달리 아무것도 '이동'시키지 않습니다.std::move는 단지 주어진 인자를 rvalue reference 타입으로 캐스팅(casting)하는 역할만 합니다.

역할: lvalue를 rvalue reference로 안전하게 변환하여, 이 변환된 rvalue reference가 이동 생성자나 이동 대입 연산자를 호출하도록 유도하는 것입니다.

사용 시점 : 이름이 있는 lvalue 변수의 자원을 다른 객체로 '이동'시키고 싶을 때 사용합니다.std::move를 사용한 후에는 원본 lvalue 변수는 '이동된 상태(moved-from state)'가 되므로, 더 이상 유효한 상태라고 가정해서는 안 되며, 파괴되거나 새로운 값을 할당하기 전까지는 사용하지 않는 것이 안전합니다.

동작 : static_cast<T&&>(arg) 와 유사합니다.

C++

std::vector<int> sourceVec(100);
// ... sourceVec에 데이터 채우기 ...

// sourceVec는 lvalue입니다.
// MyVector targetVec = sourceVec; // -> 복사 생성자 호출
MyVector targetVec = std::move(sourceVec); // -> 이동 생성자 호출
// std::move(sourceVec)는 sourceVec를 MyVector&& 타입으로 캐스팅하고,
// MyVector&&를 인자로 받는 이동 생성자가 호출되어 자원을 훔쳐갑니다.

// 이 시점에서 sourceVec는 '이동된 상태'이므로 유효하지 않을 수 있습니다.
// sourceVec.empty()는 true일 수 있고, 내부 데이터는 nullptr일 수 있습니다.
*/
/*
6. std::forward(그리고 유니버설 레퍼런스 / 전달 참조)
std::forward는 이동 시맨틱과 함께 * *템플릿을 통한 완벽 전달(Perfect Forwarding) * *을 가능하게 하는 핵심 도구입니다.이 부분이 가장 복잡하게 느껴질 수 있습니다.

a.유니버설 레퍼런스(Universal Reference) 또는 전달 참조(Forwarding Reference)
T && 문법은 두 가지 의미로 사용될 수 있습니다 :

rvalue reference : (위에서 설명했듯이) 일반적인 함수에서 T && 는 오직 rvalue에만 바인딩됩니다.

유니버설 레퍼런스(또는 전달 참조) : 템플릿 함수 인자의 추론 과정에서 T && 가 사용될 때 이 특별한 의미를 가집니다.

template<typename T> void func(T && arg) 와 같은 형태로 사용될 때, T&& 는 lvalue와 rvalue 모두에 바인딩될 수 있습니다.

추론 규칙 :

만약 func에 lvalue가 전달되면(예 : func(my_lvalue);), T는 lvalue reference 타입(X&)으로 추론됩니다.이때 참조 붕괴(Reference Collapsing) 규칙에 따라(X&) && 는 X& 가 됩니다.즉, arg는 lvalue reference가 됩니다.

만약 func에 rvalue가 전달되면(예 : func(create_rvalue());), T는 non - reference 타입(X)으로 추론됩니다.이때(X) && 는 X && 가 됩니다.즉, arg는 rvalue reference가 됩니다.

결과적으로, T&& 는 lvalue가 오면 lvalue reference로, rvalue가 오면 rvalue reference로 변신하여 원본 인자의 lvalue / rvalue 속성을 그대로 유지합니다.

b.std::forward 의 역할
std::forward는 유니버설 레퍼런스의 이 특성을 활용하여, ** 원본 인자의 lvalue / rvalue 속성을 그대로 다음 함수 호출로 '전달(forward)' * *하는 역할을 합니다.

목적: 유니버설 레퍼런스로 받은 인자를 다른 함수에 전달할 때, 원본 인자가 lvalue였다면 lvalue로, rvalue였다면 rvalue reference로 정확하게 전달하여 복사를 피하고 이동을 가능하게 합니다.

동작 방식 : std::forward<T>(arg)는 T의 타입에 따라 arg를 static_cast<T&&>(arg) 또는 static_cast<T&>(arg)로 조건부 캐스팅합니다.

예시 :

    C++
*/
#include <iostream>
#include <string>
#include <utility> // std::move, std::forward를 위해

    void processValue(std::string & s) {
    std::cout << "  processValue(lvalue): " << s << std::endl;
}

void processValue(std::string&& s) {
    std::cout << "  processValue(rvalue): " << s << std::endl;
}

// ⭐️ 템플릿 함수에서 T&&는 유니버설 레퍼런스가 됩니다.
template<typename T>
void wrapper(T&& arg) {
    std::cout << "wrapper 호출됨: ";
    // arg는 현재 wrapper 함수 내에서는 이름이 있는 변수이므로 lvalue로 간주됩니다.
    // 따라서 std::forward를 사용하지 않으면 항상 lvalue 참조 버전의 processValue가 호출됩니다.

    // std::forward<T>(arg)를 사용하여 원본 인자의 lvalue/rvalue 속성을 유지한 채 전달
    processValue(std::forward<T>(arg));
}

int main() {
    std::string s1 = "Hello"; // s1은 lvalue
    wrapper(s1);              // s1은 lvalue -> T는 std::string&로 추론 -> std::forward<std::string&>(s1) -> std::string& (lvalue) 전달
    std::cout << std::endl;

    wrapper(std::string("World")); // std::string("World")는 rvalue -> T는 std::string으로 추론 -> std::forward<std::string>(rvalue) -> std::string&& (rvalue reference) 전달
    std::cout << std::endl;

    std::string s2 = "C++";
    wrapper(std::move(s2));   // std::move(s2)는 rvalue reference -> T는 std::string으로 추론 -> std::string&& (rvalue reference) 전달
    std::cout << std::endl;

    return 0;
}
/*
실행 결과 :

wrapper 호출됨 : processValue(lvalue) : Hello

wrapper 호출됨 : processValue(rvalue) : World

wrapper 호출됨 : processValue(rvalue) : C++
std::forward 덕분에 wrapper 함수는 자신이 받은 인자가 lvalue였는지 rvalue였는지에 따라 processValue의 적절한 오버로딩 버전을 호출할 수 있게 됩니다.이것이 바로 * *"완벽 전달" * *의 핵심입니다.
*/

//T&& 가 템플릿 함수의 매개변수로 사용될 때, 그리고 T가 추론되는 타입일 경우, 이를 유니버설 참조(Universal Reference) 또는** 전달 참조(Forwarding Reference)** 라고 부릅니다.
//
//이 경우, T&& 에 T인 객체가 들어온다고 하셨는데, 여기서 'T인 객체'가 * *좌측값(lvalue) * *인지 * *우측값(rvalue) * *인지에 따라 T의 추론 결과가 달라집니다.
//
//일반적으로 C++에서 '객체'라고 하면 int x; 나 MyClass obj; 와 같이 이름이 있고 메모리 주소를 가지는 좌측값을 의미하는 경우가 많습니다.이 가정 하에 설명드리겠습니다.
//
//T&& 에 좌측값(lvalue) 객체가 들어올 때의 추론 과정
//template<typename T> void func(T&& arg); 와 같은 함수가 있고,
//MyClass obj; 와 같은** 좌측값 obj** 를 func(obj); 호출로 넘길 때의 추론 과정입니다.
//
//매개변수 타입 arg는 T&& : T && 는 유니버설 참조이므로 좌측값과 우측값 모두에 바인딩될 수 있습니다.
//
//전달된 인자는 MyClass 타입의 좌측값 obj : obj는 MyClass& 타입으로 간주됩니다.
//
//T 타입 추론 : 컴파일러는 T&& 가 MyClass& 에 바인딩될 수 있도록 T를 추론해야 합니다.이때 유니버설 참조의 특별한 추론 규칙이 적용됩니다 :
//
//좌측값이 전달되면 T는 T& 타입으로 추론됩니다.
//
//따라서 T는 MyClass& 로 추론됩니다.
//
//참조 축약(Reference Collapsing) 규칙 적용 : 이제 추론된 T 타입(MyClass&)을 원래 매개변수 타입 T&& 에 대입합니다 :
//
//T &&
//rightarrow(MyClass&) &&
//
//참조 축약 규칙에 따라 X&&& 는 X& 가 됩니다.
//
//그러므로(MyClass&) && 는 최종적으로 MyClass& 가 됩니다.
//
//결론: T && 에 MyClass 타입의 좌측값 객체(obj)가 들어오면, T는 MyClass& 로 추론됩니다.그리고 func 함수의 매개변수 arg의 최종 타입은 MyClass& 가 됩니다.
//
//예시
//C++

#include <iostream>
#include <type_traits> // std::is_same_v를 위해

class MyClass {};

template<typename T>
void func(T&& arg) {
    // T의 추론된 타입을 출력
    if (std::is_same_v<T, MyClass>) {
        std::cout << "T는 MyClass (비-참조)로 추론됨" << std::endl;
    }
    else if (std::is_same_v<T, MyClass&>) {
        std::cout << "T는 MyClass& (좌측값 참조)로 추론됨" << std::endl;
    }
    else if (std::is_same_v<T, MyClass&&>) {
        std::cout << "T는 MyClass&& (우측값 참조)로 추론됨" << std::endl;
    }

    // arg의 실제 타입을 출력
    if (std::is_same_v<decltype(arg), MyClass>) {
        std::cout << "arg의 실제 타입은 MyClass (비-참조)" << std::endl;
    }
    else if (std::is_same_v<decltype(arg), MyClass&>) {
        std::cout << "arg의 실제 타입은 MyClass& (좌측값 참조)" << std::endl;
    }
    else if (std::is_same_v<decltype(arg), MyClass&&>) {
        std::cout << "arg의 실제 타입은 MyClass&& (우측값 참조)" << std::endl;
    }

    // std::forward 사용 예시: 인자의 lvalue/rvalue성을 유지하여 전달
    // func_target(std::forward<T>(arg));
}

int main() {
    MyClass obj;             // 좌측값 객체
    func(obj);               // func에 좌측값 전달
    std::cout << "--------------------" << std::endl;
    func(MyClass{});         // func에 우측값 (임시 객체) 전달
}