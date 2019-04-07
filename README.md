
## WISH shell
This code creates an interface called wish. Wish issues a prompt and allows functionality found in most shell programs.

# How to Use

Wish contains two modes of use: 
    1) In the interactive mode in which the user is prompted to input command line by line. 
    2) In the batch mode where the user can pass a file containing instructions instead of issuing them line by line. 

# Functionality 1: Setting Path(s) of User Functions

The user has the ability to specify the paths to files that can be accessed directly by the wish. They may change directories with the cd command. They can also see their recent calls with the history command.

# Functionality 2: Built-in Commands

The user may exit at any time by calling exit. They may change their working directory by running cd (new directory). To check commands that were recently run, the user may type history. To see the last n commands, they can type history n.

The user may elect to redirect their output to another file rather than the standard output with the > command. They may also elect to execute a command with the output of another command. This can be done with separating the two arguments with the |. 
All errors will present "An error has occured\n" as the error message.
