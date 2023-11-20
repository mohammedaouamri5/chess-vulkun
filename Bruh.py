import sys
import subprocess
from PyQt5.QtWidgets import QApplication, QMainWindow, QPushButton, QHBoxLayout, QWidget, QComboBox
import os
import shutil
from glob import glob  




class BatchScriptRunner(QMainWindow):
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        self.setWindowTitle('Batch Script Runner')
        self.setGeometry(100, 100, 600, 200)

        self.central_widget = QWidget(self)
        self.setCentralWidget(self.central_widget)

        self.layout = QHBoxLayout()  # Use QHBoxLayout to stack buttons horizontally

        # ComboBox for selecting the batch script
        self.box1 = QComboBox(self)
        self.box1.addItems(['Debug', 'Dist',  'Realis'])
        self.layout.addWidget(self.box1)

        # ComboBox for selecting the action to perform
        self.box2 = QComboBox(self)
        self.box2.addItems( [ "BUILD",  "RUN",  'CLEAR'])
        self.layout.addWidget(self.box2)

        # Button to execute the selected action
        self.run_button = QPushButton('Run', self)
        self.run_button.clicked.connect(self.execute_selected_action)
        self.layout.addWidget(self.run_button)

        self.central_widget.setLayout(self.layout)

        # Apply dark mode stylesheet
        self.setStyleSheet("""
            QMainWindow {
                background-color: #1E1E1E;
                color: #FFFFFF;
            }
            QComboBox, QPushButton {
                background-color: #333;
                color: #FFFFFF;
                border: 2px solid #666;
                border-radius: 10px; /* Rounded edges */
                font-size: 40px; /* Increase font size */
                padding: 10px; /* Increase padding to make widgets larger */
                font-family: Italic, sans-serif; /* Change font */
            }
            QComboBox:hover, QPushButton:hover {
                background-color: #555;
            }
            
            QComboBox::drop-down {
                subcontrol-origin: padding;
                subcontrol-position: right center;
                width: 40px;
                border: none;
            }
            QComboBox QAbstractItemView {
                background-color: #333;
                color: #FFFFFF;
                border: 2px solid #666;
                border-radius: 10px;
                font-size: 40px; /* Increase font size */
                font-family: Italic, sans-serif; /* Change font */
            }
        """)

    def execute_selected_action(self):
        action = self.box2.currentText()
        config = self.box1.currentText()

        FUNS = {
            "RUN": RUN,
            "BUILD": BUILD,
            "CLEAR": CLEAR
        }

        if action in FUNS:
            FUNS[action](config)

def call(command) -> None:
    
    try:
        subprocess.run(
            ["powershell", command],
            check=True,
            text=True
        )
    except Exception as e:
        print("Error executing " ,  e ) 
        raise e

def RUN(config: str) -> None:

    source_folder = "./chess"
    destination_folder = f"./bin/{config}-windows-x86_64/chess"

    print("============Copying============")
    for folder_name in os.listdir(source_folder):
        source_path = os.path.join(source_folder, folder_name)

        if os.path.isdir(source_path) and folder_name.lower() != "src":
            destination_path = os.path.join(destination_folder, folder_name)

            if not os.path.exists(destination_path):
                shutil.copytree(source_path, destination_path)
                print(f"Copying {source_path} to {destination_path}")

    print("============RUN============")
    # Run the executable
    os.chdir(f"./bin/{config}-windows-x86_64/chess")
    os.system("./chess.exe")

def BUILD(config: str) -> None:
    call(f"msbuild chess.sln /p:Configuration={config}")

def CLEAR(config: str) -> None:
    call(f"msbuild chess.sln /t:Clean /p:Configuration={config}")

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = BatchScriptRunner()
    window.show()
    sys.exit(app.exec_())
    print(*glob("*.sln"))
