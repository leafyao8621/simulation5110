d = read.csv("test.csv", header = FALSE)[,1]
print(mean(d))
hist(d, prob = TRUE, breaks = 10)
