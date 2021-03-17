# Documentation Gtest in c++

## Presentation

Google test is a working framework where you can test the output of whole functions or parts of code.

It is widely used because it allows in a fast and fairly intuitive way to write tests that "do not hinder" the actual code.

These tests are largely functional and manageable.

## Basic concepts

### Type of test

First of all, the tests can be of two types: fatal or non-fatal.

1. Fatal tests: they indicate tests that if they fail interrupt the entire function and even if within the function another test is true, it will be displayed as incorrect, therefore false.

2. Non-fatal tests: this type of test, even in the event of an error, therefore in a false state, does not interrupt the function and continue the execution of the function itself.

Obviously with the consequences of having a wrong final output, or obtained in an undesirable way.

### True test or false test

Successful test: these test results indicate that the output that has been searched for is correct, and obtained. The output of these tests is = "true".

Incorrect test: if a test is = false, then it means that the result you were looking for was not obtained.

This can happen if the output of the function is different from the test, or if there are multiple tests to verify the function, and one of these is a "fatal test" and fails.

This will also lead to our test failing, which would normally have succeeded.

### Assertions

1. Fatal test: for fatal tests `ASSERT_*` is used (where "*" indicates the type of operator that is used to verify the output).

2. Non-fatal test: for fatal test `EXCPECT_*` is used (where "*" indicates the type of operator that is used to verify the output).

Remember, when they fail, `ASSERT_*` yields a fatal failure and returns from the current function, while `EXPECT_*` yields a nonfatal failure, allowing the function to continue running.

In either case, an assertion failure means its containing test fails.

#### Bool condition Assertions

- Fatal test:

  - expression is `ASSERT_TRUE(condition)` where we must verify that the output bool (condition) is indeed true.

  - expression is `ASSERT_FALSE(condition)` where we must verify that the output bool (condition) is indeed false.

- Non-fatal test:
  - expression is `EXPECT_TRUE(condition)` where we must verify that the output bool (condition) is indeed true.

  - expression is `EXPECT_FALSE(condition)` where we must verify that the output bool (condition) is indeed false.

#### Binary comparison Assertions

- Fatal test:

  - expression is `ASSERT_EQ(var1, var2)` where we must verify that `var1` is equal to `var2`. ---> `(var1 == var2)`

  - expression is `ASSERT_NE(var1, var2)` where we must verify that `var1` is different to `var2`. ---> `(var1 != var2)`

  - expression is `ASSERT_LT(var1, var2)` where we must verify that `var1` is less than `var2`. ---> `(var1 < var2)`

  - expression is `ASSERT_LE(var1, var2)` where we must verify that `var1` is less than or equal to `var2`. ---> `(var1 <= var2)`

  - expression is `ASSERT_GT(var1, var2)` where we must verify that `var1` is greater than to `var2`. ---> `(var1 > var2)`

  - expression is `ASSERT_GE(var1, var2)` where we must verify that `var1` is greater than or equal to var2. ---> `(var1 >= var2)`

- Non-fatal test:

  - expression is `EXPECT_EQ(var1, var2)`where we must verify that var1 is equal to var2.              ---> (var1 == var2)

  - expression is `EXPECT_NE(var1, var2)` where we must verify that var1 is different to var2.          ---> (var1 != var2)

  - expression is `EXPECT_LT(var1, var2)` where we must verify that var1 is minor to var2.              ---> (var1 < var2)

  - expression is `EXPECT_LE(var1, var2)` where we must verify that var1 is minor or equal to var2.     ---> (var1 <= var2)

  - expression is `EXPECT_GT(var1, var2)` where we must verify that var1 is major to var2.              ---> (var1 > var2)

  - expression is `EXPECT_GE(var1, var2)` where we must verify that var1 is major or equal to var2.     ---> (var1 >= var2)

#### String comparison Assertions

- Fatal test:

  - expression is `ASSERT_STREQ(str1,str2)` where we must verify that str1 has the same content of str2.                                ---> (str1 == str2)

  - expression is `ASSERT_STRNE(str1,str2)` where we must verify that str1 has different content than str2.                             ---> (str1 != str2)

  - expression is `ASSERT_STRCASEEQ(str1,str2)` where we must verify that str1 has the same content of str2, ignoring the case          ---> ((str1 == str2)ignoring case)

  - expression is `ASSERT_STRCASENE(str1,str2)` where we must verify that str1 has different content than str2, ignoring the case       ---> ((str1 <= str2)ignoring case)

- Non-fatal test:

  - expression is `EXPECT_STREQ(str1,str2)` where we must verify that str1 has the same content of str2.                                ---> (str1 == str2)

  - expression is `EXPECT_STRNE(str1,str2)` where we must verify that str1 has different content than str2.                             ---> (str1 != str2)

  - expression is `EXPECT_STRCASEEQ(str1,str2)` where we must verify that str1 has the same content of str2, ignoring the case          ---> ((str1 == str2)ignoring case)

  - expression is `EXPECT_STRCASENE(str1,str2)` where we must verify that str1 has different content than str2, ignoring the case       ---> ((str1 <= str2)ignoring case)

### Simple testing

To write a simple test in C ++, you need to create the "function" `TEST()`, which will have no return.

In fact, the actual return will be determined by the various assertions. If any assertion is = false (whether it is fatal or non-fatal) and then the test will fail, otherwise it will be correct and succeeds.

#### Tests arguments

The test will contain, as the first parameter let's say, the "test group", then the TestSuiteName and then the name of the actual test, the TestName. It is preferable not to use "_" in the names.

#### Test format and simple example

I write a function that fetches from the class database of "Classeviva" the grades of the student "Donnypaolo" and I want to verify that his math grade is equal to 7 while his Italian grade is greater than 4, the text for creating the tests is the following:

1. First I'll write the function to collect the votes:

```cpp
int GradesDonnyMath(){
    body of function that return the grade in math of donnypaolo;
return MathGrade;
}
int GradesDonnyItalian(){
    body of function that return the grade in italian of donnypaolo
return ItalianGrade;
}

int main(){
    int MathGrade, ItalianGrade;
    MathGrade = GradesDonnyMath();
    ItalianGrade = GradesDonnyItalian()
}
```

2. After that, i'll write the two test to verify that the votes collected are correct (all of this will be done in another file in our case ):

```cpp
TEST(TestGrade,TestMathGrade){
    ASSERT_EQ(MathGrade, 7);
}
TEST(TestGrade,TestItalianGrade){
    EXPECT_GT(ItalianGrade, 4);
}
```

(in this case, for me, it's better to write assert(fatal test) in mathgrade test, because if the test fail, the another test fail too. This indicates that the first test fail, there isn't much left to test anyway, Becuase the two query function are the same)

#### Test fixtures

A test fixtures is used when you must write many tests that operate on similar data.

A test fixtures allow to reuse the same configuration of object in different tests.

How to write code with a "Test fixture class":

1. Create the "TestFixture" class or whatever you want to call it. The class must be derived from :: testing :: Test and must have "Protected" access.

2. Inside the classroom create the objects that will be needed for your tests.

3. Next we will set a base constructor with `SetUp()` if necessary and an override.

4. If necessary, use a `TearDown()` as a deconstructor, to drop all the data you allocated with `SetUp()`.

Definition of a TextFixture class:

```cpp
class FixtureClassTest : public :: testing :: Test{
    protected:
        myTestFixture1( ) { 
            // initialization code here
        } 
        void SetUp() override {
            // code here will execute just before the test ensues 
        }
        void TearDown() {
            // code here will be called just after the test completes
            // ok to through exceptions from here if need b    
        }
}
```

5. In the test we will use `TEST_F()` instead of `TEST()` and the TextFixture's name in the TestSuiteName camp.

```cpp
TEST_F(FixtureClassTest,Test1){
    //in this test we will use the object declareted in the TestFixture Class
    //for example, if we have declared an object objnumber1, with attributes like id, we could do a test of this type 
    EXPECT_EQ(objnumber1.id,2);
    //otherwise we can initialing a var in the test with the value of the object's attribute
    int var1 = objnumber1.id;
    EXPECT_GT(var1,0);
}
```

I'm not sure if you write it like this but as soon as I understand how to use these TestFixture classes I will return :)

the basic idea, however, is to use objects only for tests and then to delete them
