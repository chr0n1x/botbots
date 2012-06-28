#include "boundjob.h"

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
    BoundJob simple(&mySimpleJob);
    Task* job = &simple;

    cout << "Call via derived class" << endl;
    simple.execute();

    cout << "Call via base class" << endl;
    job->execute();

    BoundJob1<int> oneArgJobI(&myOneArgJob, 5);
    job = &oneArgJobI;

    cout << "Call one arg (int) via derived" << endl;
    oneArgJobI.execute();

    cout << "Call one arg (int) via base" << endl;
    job->execute();

    BoundJob1<string> oneArgJobS(&myOneArgJob,
                                 "A cortex simply holds task pointers and can"
                                 " then thread any number of arguments!");
    job = &oneArgJobS;

    cout << "Call one arg (double) via derived" << endl;
    oneArgJobS.execute();

    cout << "Call one arg (double) via base" << endl;
    job->execute();


    BoundJob2<string, int> twoArg(&twoArgJob, "Five + Seven = ", 12);
    job = &twoArg;

    cout << "Call two arg via derived" << endl;
    twoArg.execute();

    cout << "Call two arg via base" << endl;
    job->execute();


    return 0;
}
