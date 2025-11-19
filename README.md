# GIA1 Tasks



Implementing a program that counts student's total grades and sums them up into a final grade using the following formula:



$$Final \\space points = 0.4 \* (Average \\space of \\space HW|Median \\space of \\space HW) + 0.6 \* Egzam$$

## Container Performance Comparison

### VECTOR
Has a contiguous memory which leads to a very fast iteration. Has the best CPU cache locality, and it's easy to pre-allocate large blocks. However, copying millions of 'Person' objects costs a lot of CPU time. Conclusion - It's fastest overall for both reading + splitting, however it's memory-heavy but cache-efficient.

### DEQUE
Has multiple contiguous blocks which leads to few reallocations. Has fast push/pop front/back and it's often close to vector in iteration speed. However, it has a slightly worse cache locality than vector, and having multiple memory blocks means extra pointer chasing. Conclusion - It's the 2nd fastest, and usually only slightly slower than vector during splitting. 

### LIST
Has a constant-time insertion and erasure anywhere. However, it has terrible cache locality. Each element is allocated separately which leads to pointer chasing. Has slowest iteration of all STL containers, and has a large overhead per element. Conclusion - It's by far the slowest, splitting takes significantly longer, and memory usage is also much higher. 

