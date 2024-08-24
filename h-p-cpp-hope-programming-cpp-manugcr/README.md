# Final deadline realease

In this pull request i have a functional version of a multi-client C++ server with rocksdb and cannyEdgeFilter for image proccessing.

Things implemented:
- Multiclient server with TCPv4 and TCPv6.
- File sending via sockets.
- RocksDB for storing database.
- Canny Edge Filter for image processing.
- nlohmann/json for json parsing.
- httplib for http server.
- OPENMP for parallelism in canny edge filter.
- Coverage reports.

Things to do:
- Improve coverage
- Improve perfomance in canny edge filter.

---

## How to run
To run the project i have created some scripts to make it easier. You can run the server and clients with the following commands:

```bash
# To run server
./scripts/build.sh

# To run clients
./scripts/launch_ipv4.sh
./scripts/launch_ipv6.sh
```

Here you can see a gif of the server running and the clients connecting to it. In this gif as an example two users connect via terminal and modify and request for the supplies

<p align="center">
  <img src="https://github.com/ICOMP-UNC/h-p-cpp-hope-programming-cpp-manugcr/assets/20894332/4b1a01e9-b2a6-409b-8a77-0690d05c8df6"><br>
  <em>Fig 1. Clients connecting via terminal.</em>
</p>

Alternatively you can use the server via http via `localhost:8082/root` here you can see a gif of a client connecting to the server via http and modify the supplies.

<p align="center">
  <img src="https://github.com/ICOMP-UNC/h-p-cpp-hope-programming-cpp-manugcr/assets/20894332/e142b5e1-8e4b-4c8b-b8ba-b1ffd991afc8"><br>
  <em>Fig 2. Web server</em>
</p>

In this release the server is able to process a satellite image using canny edge filter submodule, and its able to sent it via sockets to a client. Here you can see a client requesting the image to the server and the server processing it, and then sending it via sockets as a zip file, that is compressed and decompressed.

<p align="center">
  <img src="https://github.com/ICOMP-UNC/h-p-cpp-hope-programming-cpp-manugcr/assets/20894332/4fb186bd-85f5-413a-a646-4148f181fd2f"><br>
  <em>Fig 3. Image processing</em>
</p>

The server keeps a log of the activity of the server, the log is private to the server here you can see an example:

```log
Fri May 17 01:20:51 2024, Connection from new client from client (127.0.0.1:42338)
Fri May 17 01:20:53 2024, Alert of possible infection in east entry.
Fri May 17 01:21:04 2024, Alert of possible infection in east entry.
Fri May 17 01:21:05 2024, Update of supplies from client from client (127.0.0.1:42338) - Authorized
Fri May 17 01:21:08 2024, Connection close from client from client (127.0.0.1:42338)
Fri May 17 01:21:12 2024, Emergency notification. Network Disruption
Fri May 17 01:21:14 2024, Connection from new client from client (::ffff:127.0.0.1:49838)
Fri May 17 01:21:17 2024, Request of state from client from client (::ffff:127.0.0.1:49838)
Fri May 17 01:21:18 2024, Alert of possible infection in south entry.
Fri May 17 01:21:20 2024, Update of supplies from client from client (::ffff:127.0.0.1:49838) - Denied invalid hostname
Fri May 17 01:21:22 2024, Connection close from client from client (::ffff:127.0.0.1:49838)
```

---

## OPENMP

To test and see the perfomance of the canny edge filter i used OPENMP to parallelize the code, instead of testing and doing the profiling with the server, i used the canny edge filter as a standalone program to test the perfomance of the parallelism, with the main.cpp inside the cannyEdgeFilter project.

To run the canny edge i made a simple script that also links `gprof` and `perf` to the program to see the perfomance of the code, and automatically generates an analysis.txt and graph of the perfomance of the code.

```bash
./build.sh
```

Here you can see a vanilla run of the canny edge filter with satellite .tiff image, as we can observe the nonMaximumSuppression is the most time consuming function, using 42% of the total time, followed by sobelOperator with 24% and applyGaussianBlur with 19%.

```bash
Flat profile:

Each sample counts as 0.01 seconds.
  %   cumulative   self              self     total           
 time   seconds   seconds    calls   s/call   s/call  name    
 42.35      2.74     2.74        1     2.74     2.74  EdgeDetection::nonMaximumSuppression()
 24.73      4.34     1.60        1     1.60     1.60  EdgeDetection::sobelOperator()
 19.94      5.63     1.29        1     1.29     1.29  EdgeDetection::applyGaussianBlur()
  7.42      6.11     0.48 113327617     0.00     0.00  EdgeDetection::checkContours(cv::Mat&, cv::Mat const&, int, int, int, int)
  4.95      6.43     0.32        1     0.32     0.80  EdgeDetection::applyLinkingAndHysteresis()
  0.62      6.47     0.04                             _init
  0.00      6.47     0.00       17     0.00     0.00  std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_M_mutate(unsigned long, unsigned long, char const*, unsigned long)
  0.00      6.47     0.00        6     0.00     0.00  frame_dummy
  0.00      6.47     0.00        2     0.00     0.00  ImageFileOperations::saveImage(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, cv::Mat const&)
  0.00      6.47     0.00        2     0.00     0.00  std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::basic_string<std::allocator<char> >(char const*, std::allocator<char> const&)
  0.00      6.47     0.00        1     0.00     5.63  EdgeDetection::cannyEdgeDetection(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&)
  0.00      6.47     0.00        1     0.00     0.00  EdgeDetection::EdgeDetection(float, float, float)
  0.00      6.47     0.00        1     0.00     0.00  EdgeDetection::~EdgeDetection()
  0.00      6.47     0.00        1     0.00     0.00  ImageFileOperations::loadImage(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&)
  0.00      6.47     0.00        1     0.00     0.00  std::ctype<char>::do_widen(char) const
  0.00      6.47     0.00        1     0.00     0.00  std::_Sp_counted_ptr_inplace<ImageFileOperations, std::allocator<ImageFileOperations>, (__gnu_cxx::_Lock_policy)2>::_M_destroy()
  0.00      6.47     0.00        1     0.00     0.00  std::_Sp_counted_ptr_inplace<ImageFileOperations, std::allocator<ImageFileOperations>, (__gnu_cxx::_Lock_policy)2>::_M_dispose()

# Total Lost Samples: 0
#
# Samples: 59K of event 'cycles'
# Event count (approx.): 45420496473
#
# Overhead  Command    Shared Object                  Symbol                                                                                        
# ........  .........  .............................  ..............................................................................................
#
    17.92%  cannyEdge  cannyEdge                      [.] EdgeDetection::nonMaximumSuppression
    16.95%  cannyEdge  libm.so.6                      [.] __atanf
     9.32%  cannyEdge  libc.so.6                      [.] __mcount_internal
     8.06%  cannyEdge  cannyEdge                      [.] EdgeDetection::sobelOperator
     8.01%  cannyEdge  cannyEdge                      [.] EdgeDetection::applyGaussianBlur
     3.92%  cannyEdge  libm.so.6                      [.] __atan2f_finite@GLIBC_2.15
     3.54%  cannyEdge  cannyEdge                      [.] EdgeDetection::checkContours
     2.71%  cannyEdge  libm.so.6                      [.] __atan2f
     2.57%  cannyEdge  cannyEdge                      [.] EdgeDetection::applyLinkingAndHysteresis
     2.51%  cannyEdge  libz.so.1.2.11                 [.] deflate
     1.62%  cannyEdge  libc.so.6                      [.] __memset_avx2_unaligned_erms
     1.13%  cannyEdge  libc.so.6                      [.] _mcount
     1.04%  cannyEdge  [kernel.kallsyms]              [k] error_entry
```

To try to improve the perfomance of the code i used OPENMP function `#pragma omp parallel for collapse(n)`. 
- pragma omp is a directive that tells the compiler to parallelize the code.
- parallel creates a team of threads that execute the code block.
- for divides the loop iterations among the threads.
- collapse(n) collapses the n loops into one.

In my case i have a maximum of 4 threads and the project has nested for loops that can be improved with the collapse(2) directive.


```bash
Flat profile:

Each sample counts as 0.01 seconds.
  %   cumulative   self              self     total           
 time   seconds   seconds    calls   s/call   s/call  name    
 96.42      8.09     8.09        2     4.04     4.04  std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::basic_string<std::allocator<char> >(char const*, std::allocator<char> const&)
  2.86      8.33     0.24        1     0.24     0.24  EdgeDetection::applyGaussianBlur()
  0.72      8.39     0.06                             _init
  0.00      8.39     0.00       16     0.00     0.00  std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_M_mutate(unsigned long, unsigned long, char const*, unsigned long)
  0.00      8.39     0.00        6     0.00     0.00  frame_dummy
  0.00      8.39     0.00        1     0.00     0.00  EdgeDetection::sobelOperator()
  0.00      8.39     0.00        1     0.00     0.24  EdgeDetection::cannyEdgeDetection(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&)
  0.00      8.39     0.00        1     0.00     0.00  EdgeDetection::nonMaximumSuppression()
  0.00      8.39     0.00        1     0.00     0.00  EdgeDetection::applyLinkingAndHysteresis()
  0.00      8.39     0.00        1     0.00     0.00  EdgeDetection::EdgeDetection(float, float, float)
  0.00      8.39     0.00        1     0.00     0.00  EdgeDetection::~EdgeDetection()
  0.00      8.39     0.00        1     0.00     0.00  ImageFileOperations::loadImage(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&)
  0.00      8.39     0.00        1     0.00     0.00  ImageFileOperations::saveImage(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, cv::Mat const&)
  0.00      8.39     0.00        1     0.00     0.00  std::ctype<char>::do_widen(char) const
  0.00      8.39     0.00        1     0.00     0.00  std::_Sp_counted_ptr_inplace<ImageFileOperations, std::allocator<ImageFileOperations>, (__gnu_cxx::_Lock_policy)2>::_M_destroy()
  0.00      8.39     0.00        1     0.00     0.00  std::_Sp_counted_ptr_inplace<ImageFileOperations, std::allocator<ImageFileOperations>, (__gnu_cxx::_Lock_policy)2>::_M_dispose()  

# Total Lost Samples: 0
#
# Samples: 144K of event 'cycles'
# Event count (approx.): 110769076974
#
# Overhead  Command    Shared Object                  Symbol                                                                                        
# ........  .........  .............................  ..............................................................................................
#
    10.26%  cannyEdge  libm.so.6                      [.] __atanf
    10.05%  cannyEdge  [kernel.kallsyms]              [k] syscall_return_via_sysret
     9.50%  cannyEdge  cannyEdge                      [.] EdgeDetection::sobelOperator
     9.43%  cannyEdge  cannyEdge                      [.] EdgeDetection::nonMaximumSuppression
     7.44%  cannyEdge  cannyEdge                      [.] EdgeDetection::applyGaussianBlur
     7.07%  cannyEdge  [kernel.kallsyms]              [k] syscall_exit_to_user_mode
     5.12%  cannyEdge  [kernel.kallsyms]              [k] entry_SYSCALL_64_after_hwframe
     3.89%  cannyEdge  [kernel.kallsyms]              [k] entry_SYSCALL_64
     3.20%  cannyEdge  libm.so.6                      [.] __atan2f_finite@GLIBC_2.15
     3.08%  cannyEdge  cannyEdge                      [.] EdgeDetection::applyLinkingAndHysteresis
     2.01%  cannyEdge  libc.so.6                      [.] malloc_consolidate
     1.57%  cannyEdge  [kernel.kallsyms]              [k] hash_futex
```

In the vanilla execution we can see that the nonMaximumSuppression function is the most time consuming function, using 42% of the total time, followed by sobelOperator with 24% and applyGaussianBlur with 19%, checkContours is called 113327617 times but it only uses uses 7.42% of the total time. 

So one thing to keep in my to improve the perfomance of the code is to get better times at nonMaximumSuppression, using this compiler directive of pragma collapse we achieved a better perfomance downgrading from 42% to 9.43% of the total time, but there was no time improvement as it appear a new time consuming function `std::__cxx11::basic_string` using 96.42% of the total time.

This constructor is taking a lot of time and it suggests that the parallelism is increasing the string handling overhead, this could be for excessive copying strings in parallel regions.

One thing to keep in mind is that its not always good to parallelize the code, as each change of context in the threads can be expensive so there is a limit to the number of threads that can be used to parallelize the code.

---

## Why only TCP?
I opted to utilize TCP exclusively, foregoing the use of UDP. This decision stems from several critical factors inherent in the nature of our application's requirements. TCP offers reliable, connection-oriented communication, ensuring that data packets are delivered in order and without loss, vital for tasks such as file transfer or database access where data integrity is paramount. Additionally, TCP's built-in error detection and retransmission mechanisms further guarantee robustness in data transmission, particularly essential for long-distance or unreliable network environments. While UDP boasts lower overhead and faster transmission speeds, its connectionless and unreliable nature make it unsuitable for our application, which demands dependable and ordered data delivery. Therefore, by harnessing TCP's reliability and resilience, i prioritize the seamless and secure exchange of information, enhancing the overall performance and user experience of our client-server system.

## Database
In this project i used json and rocksdb to handle the supplies database. I used nhlohmann/json to parse the json file and rocksdb to store the data, when i first create the database at the server setup i use json to parse and create the file, then i use rocksdb to store the data and make the queries. RocksDB provides a persistent key-value store for fast storage and retrieval of data, offering high performance and scalability for large datasets. By leveraging RocksDB's efficient storage engine, i ensure rapid access to the supplies database, enhancing the server's responsiveness and user experience.

I didnt used the rocksDbWrapper because for a small project like this i think its relatively straighforward to use rocksdb directly, without adding a problem in between, as at the time i started the project the wrapper wasnt fully implemented yet.

---

## Coverage
I used gcov and lcov to generate the coverage reports of the libraries of the project, this is the current state of the coverage of the project.

```bash
------------------------------------------------------------------------------
                           GCC Code Coverage Report
Directory: .
------------------------------------------------------------------------------
File                                       Lines    Exec  Cover   Missing
------------------------------------------------------------------------------
lib/libdyn/src/emergency.cpp                  31      15    48%   ...
lib/libsta/src/alerts.cpp                     42      26    61%   ...
lib/libsta/src/supplies.cpp                  146      63    43%   ...
------------------------------------------------------------------------------
TOTAL                                        219     104    47%
------------------------------------------------------------------------------
```

---

Fixes:
- Eliminated unused comments.
- Fixed logic in supplies
- OpenMP Logic

---

## OpenMP
With this new paralelism implentation i improved the performance of canny edge by 21% (7.24 to 5.74 seconds) when i want to perform a canny edge filter to my image.

This improvement was obtained by using openmp inside the nonMaximumSuppression function which was the one that was taking the most time to finish.

```c
void EdgeDetection::nonMaximumSuppression()
{
    std::cout << "3/4: Non maximum suppression." << std::endl;

    int rows = m_magnitude.rows;
    int cols = m_magnitude.cols;

    float neighbor_q = 255;
    float neighbor_r = 255;
    float angle;
    float central;

    #pragma omp parallel for num_threads(NUM_THREADS) collapse(2) private(neighbor_q, neighbor_r, angle, central)
    for (int i = 1; i < rows - 1; ++i)
    {
        // code here...
    }

    // Handle borders separately
    m_cannyEdges.row(0).setTo(0);
    m_cannyEdges.row(rows - 1).setTo(0);
    m_cannyEdges.col(0).setTo(0);
    m_cannyEdges.col(cols - 1).setTo(0);

    m_imageFileOperations->saveImage("maxsupress.png", m_cannyEdges);
}
```

By using this `pragma omp parallel` with now 4 threads instead of just 1 with a vanilla implementation, and by declaring the variables used inside the loop, now outside the performance was better.

Here we can see the vanilla performance of the cannyEdgeFilter.cpp using `gprof`
```
Each sample counts as 0.01 seconds.
  %   cumulative   self              self     total           
 time   seconds   seconds    calls   s/call   s/call  name    
 38.37      2.82     2.82        1     2.82     2.82  EdgeDetection::nonMaximumSuppression()
 22.31      4.46     1.64        1     1.64     1.64  EdgeDetection::sobelOperator()
 21.09      6.01     1.55        1     1.55     1.55  EdgeDetection::applyGaussianBlur()
  9.66      6.72     0.71        1     0.71     1.29  EdgeDetection::applyLinkingAndHysteresis()
  7.89      7.30     0.58 113327617     0.00     0.00  EdgeDetection::checkContours(cv::Mat&, cv::Mat const&, int, int, int, int)
  0.68      7.35     0.05                             _init
  0.00      7.35     0.00       16     0.00     0.00  std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_M_mutate(unsigned long, unsigned long, char const*, unsigned long)
  0.00      7.35     0.00        6     0.00     0.00  frame_dummy
  0.00      7.35     0.00        2     0.00     0.00  std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::basic_string<std::allocator<char> >(char const*, std::allocator<char> const&)
  0.00      7.35     0.00        1     0.00     6.01  EdgeDetection::cannyEdgeDetection(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&)
  0.00      7.35     0.00        1     0.00     0.00  EdgeDetection::EdgeDetection(float, float, float)
  0.00      7.35     0.00        1     0.00     0.00  EdgeDetection::~EdgeDetection()
  0.00      7.35     0.00        1     0.00     0.00  ImageFileOperations::loadImage(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&)
  0.00      7.35     0.00        1     0.00     0.00  ImageFileOperations::saveImage(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, cv::Mat const&)
  0.00      7.35     0.00        1     0.00     0.00  std::ctype<char>::do_widen(char) const
  0.00      7.35     0.00        1     0.00     0.00  std::_Sp_counted_ptr_inplace<ImageFileOperations, std::allocator<ImageFileOperations>, (__gnu_cxx::_Lock_policy)2>::_M_destroy()
  0.00      7.35     0.00        1     0.00     0.00  std::_Sp_counted_ptr_inplace<ImageFileOperations, std::allocator<ImageFileOperations>, (__gnu_cxx::_Lock_policy)2>::_M_dispose()
```

And here is the new version:
```
Each sample counts as 0.01 seconds.
  %   cumulative   self              self     total           
 time   seconds   seconds    calls   s/call   s/call  name    
 37.98      2.18     2.18        2     1.09     1.09  std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::basic_string<std::allocator<char> >(char const*, std::allocator<char> const&)
 24.91      3.61     1.43        1     1.43     1.43  EdgeDetection::sobelOperator()
 22.13      4.88     1.27        1     1.27     1.27  EdgeDetection::applyGaussianBlur()
  8.89      5.39     0.51 113327617     0.00     0.00  EdgeDetection::checkContours(cv::Mat&, cv::Mat const&, int, int, int, int)
  5.40      5.70     0.31        1     0.31     0.82  EdgeDetection::applyLinkingAndHysteresis()
  0.70      5.74     0.04                             _init
  0.00      5.74     0.00       16     0.00     0.00  std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_M_mutate(unsigned long, unsigned long, char const*, unsigned long)
  0.00      5.74     0.00        6     0.00     0.00  frame_dummy
  0.00      5.74     0.00        1     0.00     2.70  EdgeDetection::cannyEdgeDetection(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&)
  0.00      5.74     0.00        1     0.00     0.00  EdgeDetection::nonMaximumSuppression()
  0.00      5.74     0.00        1     0.00     0.00  EdgeDetection::EdgeDetection(float, float, float)
  0.00      5.74     0.00        1     0.00     0.00  EdgeDetection::~EdgeDetection()
  0.00      5.74     0.00        1     0.00     0.00  ImageFileOperations::loadImage(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&)
  0.00      5.74     0.00        1     0.00     0.00  ImageFileOperations::saveImage(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, cv::Mat const&)
  0.00      5.74     0.00        1     0.00     0.00  std::ctype<char>::do_widen(char) const
  0.00      5.74     0.00        1     0.00     0.00  std::_Sp_counted_ptr_inplace<ImageFileOperations, std::allocator<ImageFileOperations>, (__gnu_cxx::_Lock_policy)2>::_M_destroy()
  0.00      5.74     0.00        1     0.00     0.00  std::_Sp_counted_ptr_inplace<ImageFileOperations, std::allocator<ImageFileOperations>, (__gnu_cxx::_Lock_policy)2>::_M_dispose()
```