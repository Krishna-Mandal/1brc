This is a fork of https://github.com/gunnarmorling/1brc and i will be doing this in Cpp.

## Start the project :

    
    git clone https://github.com/Krishna-Mandal/1brc.git
    ./mvnw clean verify
    ./create_measurements.sh 1000000000
    
This will get you started with a measurement file having 1 Billion of data.

## 1st version :
This was just  apartial implementation to read one billion lines one by one by getlne method. This does not calculate anything else so far.
Since it contains so much of I/O, it is very slow, so slow, that i had to kill process.

## 2nd version :
Reading a chunk of memory (1MB), instead of each line. Significant improvemt in timing.

    
    Total lines read: 1000000000
    
    real	0m39.396s
    user	0m37.915s
    sys	0m1.479s

## 3rd version :
Reading 10 MB data time.Batch processing of files.
Less frequent data update in global map.

    real	1m32.764s
    user	10m49.175s
    sys	0m12.184s
