from flask import Flask, render_template, request, session, redirect, url_for, jsonify
import sqlite3
import subprocess

app = Flask(__name__)
app.secret_key = 'your_secret_key'  # Needed for session management

# -------------------- DATABASE SETUP --------------------

def get_db_connection():
    conn = sqlite3.connect('users.db')
    conn.row_factory = sqlite3.Row
    return conn

# Create users table if it doesn't exist
def init_db():
    conn = get_db_connection()
    conn.execute('''
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            userid TEXT UNIQUE NOT NULL,
            password TEXT NOT NULL,
            email TEXT NOT NULL
        )
    ''')
    conn.commit()
    conn.close()

# -------------------- AUTHENTICATION --------------------

@app.route('/')
def index():
    return render_template('login.html')

@app.route('/login', methods=['POST'])
def login():
    userid = request.form.get('userid')
    password = request.form.get('password')
    email = request.form.get('email')

    if not userid or not password or not email:
        return jsonify({"status": "error", "message": "Please fill all the fields."})

    conn = get_db_connection()
    user = conn.execute('SELECT * FROM users WHERE userid = ?', (userid,)).fetchone()

    if user:
        if user['password'] == password and user['email'] == email:
            session['userid'] = userid
            conn.close()
            return jsonify({"status": "success", "message": "Login successful!", "redirect_url": url_for('map_page')})
        else:
            conn.close()
            return jsonify({"status": "error", "message": "Invalid password or email."})
    else:
        try:
            conn.execute('INSERT INTO users (userid, password, email) VALUES (?, ?, ?)',
                         (userid, password, email))
            conn.commit()
            session['userid'] = userid
            conn.close()
            return jsonify({"status": "success", "message": "First time login successful!", "redirect_url": url_for('map_page')})
        except sqlite3.IntegrityError:
            conn.close()
            return jsonify({"status": "error", "message": "User ID already exists. Please try a different one."})

# -------------------- MAIN MAP PAGE --------------------

@app.route('/map')
def map_page():
    return render_template('map.html')

# -------------------- PATH FINDING (Integrated from pathconnect.py) --------------------

@app.route('/find_path', methods=['POST'])
def find_path():
    algorithm = request.form['choice']
    source = request.form['source']
    destination = request.form['destination']

    try:
        result = subprocess.run(
            ['integrated.exe', algorithm, source, destination],
            capture_output=True,
            text=True,
            check=True
        )
        output = result.stdout

        # Extract the OpenStreetMap URL (line starting with http)
        osm_link = None
        for line in output.splitlines():
            if line.startswith("http"):
                osm_link = line.strip()
                break

    except subprocess.CalledProcessError as e:
        output = "Error calculating path: " + str(e)
        osm_link = None

    return render_template('map.html', result=output , osm_link=osm_link)

# -------------------- CAMPUS ZONE PAGES --------------------

@app.route('/gate1')
def gate1():
    return render_template('gate1.html')

@app.route('/btech')
def btech():
    return render_template('btech.html')

@app.route('/chanakya')
def chanakya():
    return render_template('chanakya.html')

@app.route('/kpblock')
def kpblock():
    return render_template('kpblock.html')

@app.route('/Parking')
def Parking():
    return render_template('Parking.html')

@app.route('/gehu')
def gehu():
    return render_template('gehu.html')

@app.route('/convention')
def convention():
    return render_template('convention.html')

@app.route('/marina')
def marina():
    return render_template('marina.html')


@app.route('/qbc')
def qbc():
    return render_template('qbc.html')

@app.route('/gate2')
def gate2():
    return render_template('gate2.html')

@app.route('/geubs')
def geubs2():
    return render_template('geubs.html')

@app.route('/Petroleum')
def Petroleum():
    return render_template('Petroleum.html')

@app.route('/girlshostel')
def girlshostel():
    return render_template('girlshostel.html')

@app.route('/aryabhatt')
def aryabhatt():
    return render_template('aryabhatt.html')

@app.route('/boys')
def boys():
    return render_template('boys.html')

@app.route('/csit')
def csit():
    return render_template('csit.html')

@app.route('/tuck')
def tuck():
    return render_template('tuck.html')

@app.route('/cafe')
def cafe():
    return render_template('cafe.html')

@app.route('/mech')
def mech():
    return render_template('mech.html')

@app.route('/param')
def param():
    return render_template('param.html')

@app.route('/santosh')
def santosh():
    return render_template('santosh.html')

@app.route('/civil')
def civil():
    return render_template('civil.html')

@app.route('/badminton')
def badminton():
    return render_template('badminton.html')

@app.route('/chatbot')
def chatbot():
    return render_template('INDEX.html')

@app.route('/ravi')
def ravi():
    return render_template('ravi.html')

# -------------------- LOGOUT --------------------

@app.route('/logout')
def logout():
    session.pop('userid', None)
    return redirect(url_for('index'))

# -------------------- SERVER START --------------------

if __name__ == '__main__':
    init_db()  # Create the database table if it doesn't exist
    app.run(debug=True)
