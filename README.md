This is a fork of https://github.com/gunnarmorling/1brc and i will be doing this in Cpp.

## Start the project :

    ```
    git clone https://github.com/Krishna-Mandal/1brc.git
    ./mvnw clean verify
    ./create_measurements.sh 1000000000
    ```
This will get you started with a measurement file having 1 Billion of data.

## 1st version :
This was just  apartial implementation to read one billion lines one by one by getlne method. This does not calculate anything else so far.
Since it contains so much of I/O, it is very slow, so slow, that i had to kill process.
