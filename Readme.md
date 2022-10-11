# ECS 150 - Project 1

ToDo:
- Fix free for inner char* for char** objects in command_pipeline (check valgrind)
- Pipelining must support multiple status codes (i.e. [0][0][0])
- Fix hardcoding of `cd` value to either 0 or 1
- Handle `"Error: command not found"` case
- Start at phase 0 and test all cases
= Finish writing report.md




-sshell@ucd$ cat /dev/urandom | base64 -w 60 | head -5
7LYdb5ttLAddfCl1chGUM2VVxL3Ct7knL1Lnot/FeqHBKIKI60b10QfO+YPb
Rrz4AyFzmhigJTg2Pzofz9DnSF6vFt244M0r7u9y9B/WEBVvZLatqCAC8jtV
KzoG2RL2Bdbc1dpilWJQdaQU7MxwtAie/i1n9HDVqxEdge+F2QUiPtZ8u1PO
k17XTeelJ9wqhNA+VokWYwt8E1Xdn7hsDPYhbLBeg1Jvn4yWyd4Q58mzw5M6
n8ReM3+H1DVE52SGgWzmOeyDyBUoLzh+PwVwtFTBeDHTPvO+Vt/k1rpJDhVA
Child did not terminate with exit -> should be "+ completed 'cat /dev/urandom | base64 -w 80 | head -5' [0][0][0]"





Notes:
- Make sure "exit > file1.txt" created file1.txt if it DNE and populates it with "Bye..."
- Handle edge cases for pwd and cd such as those with pipelining (since we call exec on entire command pipeline object)