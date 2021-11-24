files=q1.py q2.py q3.py utils.py 111801042-lab11.pdf

.PHONY: submission
submission:
	zip -r 111801042-lab11.zip ${files}

.PHONY: eval
eval:
	@./eval.sh

clean:
	rm -f *.zip *.pri *.pub \
		output.txt secret.txt

