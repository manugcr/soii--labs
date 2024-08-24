#pragma once

/**
 * @brief Check the return value of a function and exit if it is -1
 *
 * @param exp The return value of the function
 * @param msg The error message to print
 */
int check(int exp, const char* msg);
