canvascii: canvascii.c
	gcc -o canvascii -std=c11 -Wall -Wextra canvascii.c

html:
	pandoc -o README.html README.md
	pandoc -o sujet.html sujet.md

clean :
	rm *.html *.o

test:
	bats check.bats
