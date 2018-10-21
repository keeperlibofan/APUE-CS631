/************************ tcp ***************************/
NAME
		tcp — trivially copyaﬁle
SYNOPSIS
		tcp source target
DESCRIPTION
		The tcp utility copies the contents of the source to target.
		If target is a directory, tcp will copy source into this directory.
EXAMPLES
		The following examples show common usage:
		tcp file1 file2
		tcp file1 dir
		tcp file1 dir/file2
		tcp file1 dir/subdir/subsubdir/file2
EXIT STATUS
		tcp exits 0 on success, and >0 if an error occurred.
SEE ALSO
		tcpm(1), lseek(2), read(2), write(2)
HISTORY
		The tcp program was ﬁrst assigned as the ﬁrst programming assignment for the class CS765 “Advanced
		Programming in the UNIX Environment” at Stevens Institute of Technology at least since around 2005.
BUGS
		- does not error on copying a file to itself
		- cannot copy a path into a dir:

		 ./a.out /etc/passwd .

		no indentation in main?

		- check return/error conditions of write(2)


/************************ tcpm ***************************/
NAME
		tcpm — trivially copy a ﬁle via mmap/memcpy
SYNOPSIS
		tcpm source target
DESCRIPTION
		The tcpm utility copies the contents of the source to target. That is, behaves entirely the same as
		tcp(1).
		Unlike tcp(1), tcpm uses mmap(2) and memcpy(2) instead of read(2) and write(2), which is why it can
		be rewarded with up to 10 extra credit points.
EXAMPLES
		The following examples show common usage:
		tcpm file1 file2
		tcpm file1 dir
		tcpm file1 dir/file2
		tcpm file1 dir/subdir/subsubdir/file2
EXIT STATUS
		tcpm exits 0 on success, and >0 if an error occurred.
SEE ALSO
		tcp(1), lseek(2), mmap(2), memcpy(2)
HISTORY
		The tcpm program was ﬁrst assigned as the ﬁrst programming assignment for extra credit for the class
		CS765 “Advanced Programming in the UNIX Environment” at Stevens Institute of Technology at least since
		around 2005.
BUGS
		- tries to map entire file
		- does not handle zero-length file

		- fails to copy a read-only file path to a file:

		 ./a.out /etc/passwd file