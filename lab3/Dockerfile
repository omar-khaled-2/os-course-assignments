
FROM ubuntu:latest


RUN apt-get update && apt-get install -y gcc


WORKDIR /usr/src/lab3


COPY . .


RUN gcc -o caltrain caltrain-runner.c



# Run the compiled program when the container starts
CMD ["./caltrain"]
