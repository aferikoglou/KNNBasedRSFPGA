SRCDIR=./src/
BINDIR=./bin/

CC=g++
COPT=-O0
WARN=-Wall

.PHONY: 
all: $(SRCDIR)knn-recommendation.cpp ## Compile the KNN recommendation system
	$(CC) $(WARN) $(COPT) -o $(BINDIR)knn-recommendation.exe $(SRCDIR)knn-recommendation.cpp

.PHONY: clean
clean:	## Clean bin directory
	rm -rf $(BINDIR)*

.PHONY: list
list:	## List all make targets
	@$(MAKE) -pRrn : -f $(MAKEFILE_LIST) 2>/dev/null | awk -v RS= -F: '/^# File/,/^# Finished Make Database/ {if ($$1 !~ "^[#.]") {print $$1}}' | egrep -v -e '^[^[:alnum:]]' -e '^$@$$' | sort

.PHONY: help
.DEFAULT_GOAL := help
help:
	@grep -h -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'
