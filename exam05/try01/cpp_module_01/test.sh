c++ -Wall -Wextra -Werror -W -std=c++98 *.cpp -o start
./start | cat -e > result.txt
diff result.txt test.txt
rm start
