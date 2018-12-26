#!/bin/bash

docker run -t --rm -v $(pwd)/articles:/book vvakame/review:2.4 /bin/bash -ci "cd /book && review-pdfmaker config.yml"
