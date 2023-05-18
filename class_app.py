# this controls the dog feeder when it is activated. It controls several routes to different urls which either the user can access to use the feeder



from flask import Flask,render_template,request,session
app = Flask(__name__)
app.config["DEBUG"] = True
app.secret_key = "super secret key"


my_servo_value = -3 #global variable. This displays how much weight the user want to put in the dog bowl
weight = 0 #global variable. This displays how much weight is currently in the dog bowl


#This the main page. The link to this is https://gogoi23.pythonanywhere.com/display_info.
#Here the user can input how many grams of food they want to feed the dog. If the user enters a
#string instead of a number the website crashes. I need to fix that.
@app.route('/')
def hello():
    return render_template("form1.html")

#This website is accessed by the dog feeder. The dog feeder sends the current amount of grams of food in
#the dog bowl in the <current_weight> parameter
@app.route('/info/<current_weight>')
def info(current_weight):
    global weight
    weight = current_weight
    return str(weight)

#This website displays to the user how much weight is in the bowl
@app.route('/display_info')
def display_info():
    return "There are " + str(weight) + " grams of food in the bowl."

#This website is accessed by the dog feeder. It is used to open and close the lid of the dog feeder remotely.
@app.route('/servo/<command>')
def servo(command):
    global my_servo_value

    #The feeder is constantly accessing this url. When the user imputs how much food they want to feed the
    # dog then my_servo_value goes from -3 to a number greater then 0. This number gets returned to the
    #feeder causing it to open the lid
    if command == "activate":
        return str(my_servo_value)

    #once the feeder puts the correct amount of food in the bowl it calls deactivate which sets my_servo_value
    # to a string. This means that this url now sends a string instead of a number greater then 0. This prompts
    # the dog feeder to lie still and not do anything.
    if command == "deactivate":
        my_servo_value = "No input entered."
        return str(my_servo_value)
    #return str(my_servo_value)




#this url gets accessed by the main page when the user inputs how much food they want.
@app.route('/data', methods = ['POST', 'GET'])
def data():
    global my_servo_value
    my_servo_value = -1

    #this if statement is not needed since it is always a post request.
    #I will be removing this if I ever work on it again but right now I
    # don't have time remove it and test all the features to see if it
    # still works.
    if request.method == 'POST':
        my_servo_value = request.form['servoval']  # pass the form field name as key
        #then take the iot value and send to servo
        # then render the template
        #return render_template('data.html',form_data = form_data)

        #this if and else statement check if the amount entered is above or below the limit.
        if ((int(my_servo_value) > 0) and (int(my_servo_value) < 150)):
            # Then make the servo or motor spin
            print("This is valid input")
        else:
            my_servo_value = "Invalid input"


        return my_servo_value
