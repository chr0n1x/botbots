#include "boundjob.h"

#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;
using namespace functional;

void mySimpleJob()
{
    cout << "mySimpleJob() Called" << endl;
}

void myOneArgJob(int number)
{
    cout << "myOneArgJob: number = " << number << endl;
}

void myOneArgJob(string value)
{
    cout << "myOneArgJob: value = " << value << endl;
}

void twoArgJob(string val, int num)
{
    cout << val << num << endl;
}

int main(int argc, char**argv)
{
    int runTestNum = -1;
    if (2 < argc) {
        runTestNum = std::atoi(argv[1]);
    }

    Task* job;

    if (0 == runTestNum || -1 == runTestNum) {
        BoundJob simple(&mySimpleJob);
        job = &simple;

        cout << "Call via derived class" << endl;
        simple.execute();

        cout << "Call via base class" << endl;
        job->execute();

        cout << "Test copy constructor" << endl;
        BoundJob copy(simple);
        copy.execute();
    }

    if (1 == runTestNum || -1 == runTestNum) {
        BoundJob1<int> oneArgJobI(&myOneArgJob, 5);
        job = &oneArgJobI;

        cout << "Call one arg (int) via derived" << endl;
        oneArgJobI.execute();

        cout << "Call one arg (int) via base" << endl;
        job->execute();

        BoundJob1<string> oneArgJobS(&myOneArgJob,
                                     "A cortex simply holds task pointers "
                                     "and can then thread functions with "
                                     "varying number of arguments!");
        job = &oneArgJobS;

        cout << "Call one arg (double) via derived" << endl;
        oneArgJobS.execute();

        cout << "Call one arg (double) via base" << endl;
        job->execute();

        cout << "Test copy constructor" << endl;
        BoundJob1<string> copy(oneArgJobS);
        copy.execute();
    }

    if (2 == runTestNum || -1 == runTestNum) {
        BoundJob2<string, int> twoArg(&twoArgJob, "Five + Seven = ", 12);
        job = &twoArg;

        cout << "Call two arg via derived" << endl;
        twoArg.execute();

        cout << "Call two arg via base" << endl;
        job->execute();

        cout << "Test copy constructor" << endl;
        BoundJob2<string, int> copy(twoArg);
        copy.execute();
    }


    return 0;
}
