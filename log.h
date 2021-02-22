//
// Created by benboby on 2021/2/22.
//

#ifndef CHATROOM_LOG_H
#define CHATROOM_LOG_H

#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

std::string currTime(){
    char tmp[64];
    time_t ptime;
    time(&ptime);
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&ptime));
    return tmp;
}

class Logger{
public:
    enum log_level{debug, info, warning, error};// 日志等级
private:
    std::ofstream outfile;    // 将日志输出到文件的流对象
    std::string path;              // 日志文件路径
    void output(std::string text, log_level act_level);            // 输出
public:
    Logger(std::string path);
    void DEBUG(std::string text);
    void INFO(std::string text);
    void WARNING(std::string text);
    void ERROR(std::string text);
    void writeclose();
};

Logger::Logger(std::string path){
    this->path = path;
    std::string tmp = "";  // 双引号下的常量不能直接相加，所以用一个string类型做转换
    std::string welcome_dialog = tmp + "[Welcome] " + __FILE__ + " " + currTime() + " : " + "=== Start logging ===\n";
    this->outfile.open(path, std::ios::out | std::ios::app);   // 打开输出文件
    this->outfile << welcome_dialog << std::flush;
}

void Logger::output(std::string text, log_level act_level){
    std::string prefix;
    if(act_level == debug) prefix = "[DEBUG]   ";
    else if(act_level == info) prefix = "[INFO]    ";
    else if(act_level == warning) prefix = "[WARNING] ";
    else if(act_level == error) prefix = "[ERROR]   ";
    else prefix = "";
    prefix += __FILE__;
    prefix += " ";
    std::string output_content = prefix + currTime() + " : " + text + "\n";
    outfile << output_content << std::flush;
}


void Logger::DEBUG(std::string text){
    this->output(text, debug);
}

void Logger::INFO(std::string text){
    this->output(text, info);
}

void Logger::WARNING(std::string text){
    this->output(text, warning);
}

void Logger::ERROR(std::string text){
    this->output(text, error);
}

void Logger::writeclose() {
    outfile.close();
}

#endif //CHATROOM_LOG_H
