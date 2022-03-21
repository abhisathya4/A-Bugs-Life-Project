# Final Project Proposal
## cs128@illinois


# Motivation
The motivation of our project is to automate the data collection of ant movements. Specifically, we are aiming to recognise when an ant interacts with a filter (a chemicaly coated white piece of paper) and store timestamps of interaction in the database. The abstract motivation of this project is to assist biology researchers at the University of Illinois by automating data collection that was previously done manually. Below is one of our target videos: 

[![Ant Example](http://img.youtube.com/vi/Sfop5X0Yj8c/0.jpg)](https://www.youtube.com/watch?v=Sfop5X0Yj8c "Ant Example")

# Executive Summary
In order to accomplish this task, we will be using the OpenCV computer vision library to facilitate the identification and tagging of the ants as they move.

# Minimum Viable Product (MVP)
We would like to be able to identify a moving ant on a screen, and mark anytime an ant interacts with a filter. For the purpose of an MVP, the filter will be a coordinate point that will be designated by us as a circle in the video. The timestamp of an ants initial touch and how long the touch lasts will then be input into a SQLite database and stored in order to facilitate visualization and further analysis by the researchers. 

# Features To Add After MVP Reached
- Write functions to recognize filters
- Write functions to track individual ants across the environment
- Digital recreation of the experiment, using black lines to model the ants and recreating their interactions with red colored filters
- Post-touch tracking of the ant

# Plan and Deadlines:

Meeting times: Wednesday at 3pm

- Sprint 1 (April 9 - April 14)
    - Research OpenCV
    - Set up communicate lines
    - Set up basic C++ OpenCV 
        - Youtube, tutorials, etc

- Sprint 2 (April 14 - April 21)
    - Start training the model
    - Begin to set up SQLite database
    - Write unit tests for function interactions between database and model

- Mid Project Report: April 21st

- Sprint 3 (April 21 - April 28) 
    - Set up SQLite database
    - Implement function interactions between database and model
    - Continue training the model

- Sprint 4 (April 28 - May 5)
    - Complete linkage of functions and model, ensuring that tests run smoothly
    - Implement some simple visualization to highlight our project.

- Final Project Report: May 5
