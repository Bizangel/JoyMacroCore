#pragma once
#include <string>
#include <iostream>
#include "WStringUtils.h"

#define LOG_DEBUG PrintScreenLogDebug
#define LOG_WARN PrintScreenLogWarn
#define LOG_ERROR PrintScreenLogError
#define LOG_INFO PrintScreenLogInfo

void PrintScreenLogDebug(std::string message);
void PrintScreenLogWarn(std::string message);
void PrintScreenLogError(std::string message);
void PrintScreenLogInfo(std::string message);
