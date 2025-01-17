// Update Date : 2024-12-23
// OS : Windows 10 64bit
// Program : Visual Studio 2022
// Version : C++20
// Configuration : Debug-x64, Release-x64

#include <iostream>
#include <typeinfo>

// https://en.cppreference.com/w/cpp/language/pack
// https://en.cppreference.com/w/cpp/language/fold

// common_type은 여러 타입 중 공통된 타입을 찾기 위해서 사용함.
// 산술 연산이나 비교 연산에서 사용될 수 있는 타입을 대상으로 한다.
// 연산했을 때 구해지는 공통된 타입을 구하기 위해서 사용한다고 보면 된다(표현 범위가 가장 넓은 자료형을 찾아줌).
// 1. std::common_type<T1, T2, ...>::type
// 2. std::common_type<Args...>::type
// 
// decay는 전달받은 타입에서 참조 및 한정자를 제거하고 순수한 타입을 얻기 위해서 사용함.
// 1. std::decay<int&>::type -> int
// 2. std::decay<const double>::type -> double
//
// remove_reference_t도 참조 타입을 제거하긴 하나,
// decay는 const나 volatile 수식을 제거 및 배열을 포인터로 변환하는 기능도 수행함.
//
// !! 참조만 제거하고 싶다면 remove_reference_t, 보다 광범위하게 일반적인 형태로 변환하고 싶다면 decay를 사용할 것. !!
//

// 폴드 표현식(fold expressions)는 C++17부터 도입된 개념
// 1. 좌측 폴드(Left Fold) : (... op expression)
// 2. 우측 폴드(Right Fold) : (expression op ...)
//
// 정확하게 표현하면 다음 4가지가 존재한다.
// 1. Unary Left Fold : (op ... args)
// 2. Unary Right Fold : (args ... op)
// 3. Binary Left Fold : (args op ...)
// 4. Binary Right Fold : (... op args)
// 
// 일단은 args와 ...을 op와 결합해서 사용하는 것이 핵심이라는 것만 알고 가도 된다.
//

// decltype(x + y)와 같은 방식은 직접적으로 표현 타입을 사용한다.
// 반면에 std::common_type은 타입을 결정하기 위한 작업을 필요로 한다.
//
// 특별한 결정 규칙이 필요하거나 혹은 정교한 무언가를 처리해야 하는 상황이 아니라면?
// decltype(x + y)와 같은 방식을 사용하도록 한다.
//
// decltype(x + y)는 간단하며 std::common_type은 처리 방식이 조금 복잡하다.
// 이는 컴파일 타임에 영향을 미칠 수 있는 요소다.
//

template <typename T, typename U>
auto Max(const T& x, const U& y) -> decltype(x + y)
{
    using DecayTypeX = typename std::decay<decltype(x)>::type;
    using DecayTypeY = typename std::decay<decltype(y)>::type;
    using CommonType = typename std::common_type<decltype(x), decltype(y)>::type;

    std::cout << "Max | DecayType : " << typeid(DecayTypeX).name() << ", " << typeid(DecayTypeY).name() << '\n';
    std::cout << "Max | CommonType : " << typeid(CommonType).name() << '\n';

    return (x > y) ? x : y;
}

template <typename T, typename U>
auto Add(const T& x, const U& y) -> decltype(x + y)
{
    using DecayTypeX = typename std::decay<T>::type;
    using DecayTypeY = typename std::decay<U>::type;
    using CommonType = typename std::common_type<T, U>::type;

    std::cout << "Add | DecayType : " << typeid(DecayTypeX).name() << ", " << typeid(DecayTypeY).name() << '\n';
    std::cout << "Add | CommonType : " << typeid(CommonType).name() << '\n';

    return x + y;
}

template <typename T, typename... Args>
auto MaxElement(const T& first, const Args&... args) -> typename std::common_type<T, Args...>::type
{
    /** 
     * C++17 : fold expressions(폴드 표현식)
     */
    // Type 출력
    using CommonType = typename std::common_type<T, Args...>::type;
    using Type = decltype((args + ... + first)); // decltype을 꼭 (())로 묶어야 한다(의미를 잘 생각해 보자!)

    std::cout << typeid(CommonType).name() << ", " << typeid(Type).name() << '\n';

    // 전달받은 타입 출력(Comma 연산자 활용)
    std::cout << typeid(first).name() << '\n';
    ((std::cout << typeid(args).name() << '\n'), ...);

    // 폴드 표현식을 사용해 최댓값 판별하기
    CommonType mx = (CommonType)first;

    // mx = max(mx, arg1), mx = max(mx, arg2), mx = max(mx, arg3), ...
    ((mx = std::max(mx, (CommonType)args)), ...);

    return mx;
}

template <typename T, typename... Args>
auto Sum(const T& init, const Args&... args) // 반환형은 알아서 추론(위에서 사용했던 것과 마찬가지로 반환형을 명시해도 됨)
{
    /**
     * C++17 : fold expressions(폴드 표현식)
     */
    // 전달받은 타입 출력(Comma 연산자 활용)
    std::cout << typeid(init).name() << '\n';
    (..., (std::cout << typeid(args).name() << '\n'));

    // 초기값과 함께 결합하기
    return (init + ... + args);
}

template <typename... Args>
using MyCommonType = typename std::common_type<Args...>::type;

void Run01()
{
    /**
     * declspec, common_type, decay 사용하기(함수 내부에 있는 것 보기)
     */
    float val1 = 10.0f;
    int   val2 = 12;
    
    /**
     * template에서는 다음과 같이 사용할 수 있음
     * using DecayType = typename std::decay<T>::type;
     */
    decltype(Max(val1, val2)) res1 = Max(val1, val2);
    auto res2 = Add(val1, val2);
    
    using Type = decltype(val1 + val2);
    
    // float으로 캐스팅된 것을 확인
    std::cout << typeid(res1).name() << " : " << res1 << '\n';
    std::cout << typeid(res2).name() << " : " << res2 << '\n';
    std::cout << typeid(Type).name() << '\n';

    // common_type 사용하기
    MyCommonType<int, long, unsigned long> myType1;
    MyCommonType<int, long, double> myType2;

    std::cout << typeid(myType1).name() << '\n';
    std::cout << typeid(myType2).name() << '\n';
}

void Run02()
{
    /**
     * fold expressions 사용하기
     */
    auto resA = MaxElement(10, 10.0f, 30.0, 999UL);

    std::cout << typeid(resA).name() << " : " << resA << '\n';

    std::cout << "----------\n";

    auto resB = Sum(10000, 10, 10.0f, 30ULL, 999UL);

    std::cout << typeid(resB).name() << " : " << resB << '\n';

    __int64;
}

int main()
{
    Run01();

    std::cout << "--------------------------------------------------\n";

    Run02();

    return 0;
}
