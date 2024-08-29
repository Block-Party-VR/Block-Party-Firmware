# Setup
In order to run this tool follow these steps:

1. ensure you're using python 3.12+. You can verify this by running 
```
python --version
``` 
in a terminal.

2. Inside of a terminal in the 
```
tools/animation-tools
``` 
folder, run: 
```
python -m venv venv
```
 to create your virtual environment.

3. Now activate the virtual environment. In powershell run 
```
venv\Scripts\activate
```
You may need to fix permission issues if this is your first time using a virtual environment. For git bash just run: 
```
source venv/Scripts/activate
```
4. Run 
```
pip install -r requirements.txt
```