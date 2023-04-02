#include "pipe_linux_process.hpp"

#ifdef LINUS_LINUX

//--------------------------------------------------------------------------------

#    include <sys/resource.h>
#    include <sys/time.h>

#    include <cstring>
#    include <fcntl.h>
#    include <unistd.h>
#    include <wait.h>

#    include "domain/error_message.hpp"

//--------------------------------------------------------------------------------

proc::PipeLinuxProcess::PipeLinuxProcess(const PipeLinuxProcess& other) noexcept
{
    *this = other;
}

//--------------------------------------------------------------------------------

proc::PipeLinuxProcess&
proc::PipeLinuxProcess::operator=(const PipeLinuxProcess& other) noexcept
{
    mParameters = other.mParameters;
    getRawParameters();
    return *this;
}

//--------------------------------------------------------------------------------

void
proc::PipeLinuxProcess::setComand(
    const std::vector<std::string>& aParameters) noexcept
{
    mParameters = aParameters;
    getRawParameters();
}

//--------------------------------------------------------------------------------

void
proc::PipeLinuxProcess::create() noexcept
{
    WRITE_LOG("Creating_process_with_name:", mRawParameters[0]);

    IORedirection();

    mChildPID = fork();
    if (mChildPID == -1)
    {
        WRITE_ERROR("PipeProcess", 10, "Linux", "Can't_create_process");
    }
    else if (!mChildPID)
    {
        dup2(mPipeA[0], STDIN_FILENO);
        dup2(mPipeB[1], STDOUT_FILENO);
        auto itt = &mRawParameters[0];
        execvp(mRawParameters[0], &mRawParameters[0]);
    }
    else
    {
    }
}

//--------------------------------------------------------------------------------
#    include <boost/date_time.hpp>

#    include "domain/date_and_time.hpp"
bool
proc::PipeLinuxProcess::run() noexcept
{
    WRITE_LOG("Runing_simple_process");
    wait(NULL);
    // int status;
    // boost::posix_time::ptime timeLocal =
    //     boost::posix_time::second_clock::local_time();
    // for (uint64_t i = 0;; ++i)
    // {
    //     if (wait4(NULL, &status, WNOHANG) != 0) break;
    //     boost::posix_time::ptime timeLocal2 =
    //         boost::posix_time::second_clock::local_time();
    //     auto dur = timeLocal2 - timeLocal;
    //     if (dur.seconds() > 30) break;
    // }
    return true;
}

//--------------------------------------------------------------------------------

std::optional<dom::Pair<uint64_t>>
proc::PipeLinuxProcess::runWithLimits() noexcept
{
    START_LOG_BLOCK("Runing_process_with_time_and_memory_evaluation");

    std::optional<dom::Pair<uint64_t>> result = {};

    uint64_t timeUsage   = 0;
    uint64_t memoryUsage = 0;

    rusage resourseUsage;
    int status;
    // auto startT = dom::DateAndTime::getCurentTimeNum();
    boost::posix_time::ptime timeLocal =
        boost::posix_time::second_clock::local_time();
    // wait4(mChildPID, &status, 0, &resourseUsage);
    for (uint64_t i = 0;; ++i)
    {
        if (wait4(mChildPID, &status, WNOHANG, &resourseUsage) != 0) break;
        // if (dom::DateAndTime::getCurentTimeNum() - startT > 10LL) break;
        boost::posix_time::ptime timeLocal2 =
            boost::posix_time::second_clock::local_time();
        auto dur = timeLocal2 - timeLocal;
        if (dur.seconds() > 6) break;
    }
    int gg = WIFEXITED(status);

    timeUsage += resourseUsage.ru_utime.tv_sec * 1'000'000;
    timeUsage += resourseUsage.ru_utime.tv_usec;
    timeUsage += resourseUsage.ru_stime.tv_sec * 1'000'000;
    timeUsage += resourseUsage.ru_stime.tv_usec;
    timeUsage /= 1000;

    WRITE_LOG("status:", status);
    WRITE_LOG("WIFEXITED:", WIFEXITED(status));
    WRITE_LOG("WEXITSTATUS:", WEXITSTATUS(status));
    WRITE_LOG("WIFSIGNALED:", WIFSIGNALED(status));
    WRITE_LOG("WTERMSIG:", WTERMSIG(status));
    WRITE_LOG("WIFSTOPPED:", WIFSTOPPED(status));

    WRITE_LOG("time_usage:", timeUsage);
    END_LOG_BLOCK("memory_usage:", memoryUsage);

    if (WIFEXITED(status))
    {
        result = {timeUsage, memoryUsage};
    }
    return result;
}

//--------------------------------------------------------------------------------

#    define BUFFER_SIZE 65336 * 10

//--------------------------------------------------------------------------------

void
proc::PipeLinuxProcess::IORedirection() noexcept
{
    WRITE_LOG("Rederecting_input_and_output_to_pipe");

    pipe(mPipeA);
    pipe(mPipeB);

    fcntl(mPipeA[0], F_SETPIPE_SZ, BUFFER_SIZE);
    fcntl(mPipeA[1], F_SETPIPE_SZ, BUFFER_SIZE);
    fcntl(mPipeB[0], F_SETPIPE_SZ, BUFFER_SIZE);
    fcntl(mPipeB[1], F_SETPIPE_SZ, BUFFER_SIZE);
}

//--------------------------------------------------------------------------------

void
proc::PipeLinuxProcess::readData(std::string& result) noexcept
{
    result.clear();
    char buf[1024];
    memset(buf, 0, sizeof(buf));
    while (read(mPipeB[0], &buf, 1024) == 1024)
    {
        result += std::string(buf);
        memset(buf, 0, sizeof(buf));
    }
    result += std::string(buf);
}

//--------------------------------------------------------------------------------

void
proc::PipeLinuxProcess::writeData(const std::string& aMessage) noexcept
{
    write(mPipeA[1], aMessage.c_str(), aMessage.size());
}

//--------------------------------------------------------------------------------

void
proc::PipeLinuxProcess::getRawParameters() noexcept
{
    for (auto& s : mParameters)
    {
        mRawParameters.emplace_back((char*)s.c_str());
    }
    mRawParameters.push_back(NULL);
}

//--------------------------------------------------------------------------------

#endif // !LINUS_LINUX
