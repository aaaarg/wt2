
unittest: $(UNITTESTS)
	@for f in $(UNITTESTS) ; \
		do ./$$f && echo "$$f: OK" || echo "$$f: KO"; \
	done

clean: ; $(RM) $(CLEANFILES)

lint: ; cpplint.py $(CPPLINT_FLAGS) $(wildcard *.cc) $(wildcard *.h)

.PHONY: clean lint
