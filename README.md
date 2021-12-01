# NTUA ECE Embedded Systems - High Level Synthesis (HLS) Exercise 01

This exercise demonstrates a movie [Recommendation System](https://en.wikipedia.org/wiki/Recommender_system) based on [K Nearest Neighbor](https://en.wikipedia.org/wiki/K-nearest_neighbors_algorithm) algorithm. The used dataset is a subset of [MovieLens](https://grouplens.org/datasets/movielens/). Students are going to use [High Level Synthesis](https://en.wikipedia.org/wiki/High-level_synthesis) in order to accelerate the recommendation system using the [SDSoC 2016.4](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vitis/archive-sdsoc.html) development environment. Finally, they will execute the accelerated application on a [Zynq-7000 ARM/FPGA SoC](https://www.xilinx.com/support/documentation/data_sheets/ds190-Zynq-7000-Overview.pdf) development board.

## Repository Structure

- **CPU** directory contains a CPU only version for the movie recommendation system. Students can experiment with different distance metrics on the full dataset.
- **FPGA** directory contains the main code of the exercise.
- **various** directory contains the Jupyter notebook that was used in order to understand the input data and create the used dataset.

The MovieLens dataset subset can be found on: https://drive.google.com/drive/folders/1m_kzCO8PBifs6wIZnb-vLuubm76F1B8f?usp=sharing
