#pragma once

#ifdef DEBUG
    #include<iostream>
    #define DEBUG(x) std::cerr << x << std::endl
#else
    #define DEBUG(x) // Do nothing
#endif