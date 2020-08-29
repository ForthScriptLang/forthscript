def fib(n):
	if n <= 1:
		return 1
	return fib(n - 1) + fib(n - 2)

i = 0
while i < 20:
	print(fib(i))
	i += 1