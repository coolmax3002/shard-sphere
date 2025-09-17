# Use a base image with C++ build tools
FROM gcc:latest

# Set working directory
WORKDIR /app

# Install any additional dependencies if needed
RUN apt-get update && apt-get install -y \
    make \
    && rm -rf /var/lib/apt/lists/*

# Copy source files and Makefile
COPY main.cpp server.cpp server.h Makefile ./

# Build the application
RUN make

# Set the default command to run your application
CMD ["./server"]
