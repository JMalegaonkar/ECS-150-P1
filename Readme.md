# ECS 150 - Project 1

ToDo:
- Fix free for inner char* for char** objects in command_pipeline (check valgrind)
- Pipelining must support multiple status codes (i.e. [0][0][0])
- Fix hardcoding of `cd` value to either 0 or 1
- Handle `"Error: command not found"` case
- Start at phase 0 and test all cases
= Finish writing report.md

Notes:
- Make sure "exit > file1.txt" created file1.txt if it DNE and populates it with "Bye..."
- Handle edge cases for pwd and cd such as those with pipelining (since we call exec on entire command pipeline object)