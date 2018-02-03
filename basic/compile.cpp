#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <array>
#include <iostream>
#include <memory>
#include <string>
#include <typeinfo>

#include "rep/c0.h"
#include "rep/r0.h"
#include "rep/x0.h"
#include "rep/x0s.h"

using namespace std;

static string to_asm(r0::P p)
{
    p.uniquify();
    c0::P c = p.flatten();
    x0s::P xs = c.select();
    x0::P x0 = xs.assign();
    x0.fix();
    string asms = x0.to_asm();
    cout << asms;
    return asms;
}

string compile(r0::P p)
{
    string woof = to_asm(p);
    string pname = "woof";
    pid_t pid;
    char namebuf[] = "/tmp/compiler-XXXXXX";
    int fd = mkstemp(namebuf);
    write(fd, (void*)woof.c_str(), woof.size());
    string name = string(namebuf) + ".s";
    string runtime = "/home/csano/lang/basic/runtime/lib.o";
    rename(namebuf, name.c_str());
    if ((pid = fork()) == -1)
    {
        exit(1);
    }
    else if (pid == 0)
    {
        char cname[name.size()+1];
        char cruntime[runtime.size()+1];
        char cpname[pname.size()+1];
        strncpy(cname, name.c_str(), name.size());
        strncpy(cruntime, runtime.c_str(), runtime.size());
        strncpy(cpname, pname.c_str(), pname.size());
        char* cmd[] = {
            "/usr/bin/gcc",
            cname,
            cruntime,
            "-o",
            cpname,
            NULL
        };
        execv("/usr/bin/gcc", cmd);
    }
    // TODO robust gcc error handling
    wait(NULL);
    unlink(name.c_str());
    return pname;
}

int compile_run(r0::P p)
{
    pid_t pid;
    string pname = "./" + compile(p);
    array<char, 128> buf;
    string result;
    shared_ptr<FILE> pipe(popen(pname.c_str(), "r"), pclose);
    if (!pipe)
    {
        cerr << "Failed to make pipe\n";
        exit(1);
    }
    while (!feof(pipe.get()))
    {
        if (fgets(buf.data(), 128, pipe.get()) != nullptr)
            result += buf.data();
    }
    int64_t ret = stol(result, nullptr);
    return ret;
}

bool test_compile(r0::P p, int expect)
{
    int actual = compile_run(p);
    return expect == actual;
}
