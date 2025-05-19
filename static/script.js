const chatBox = document.getElementById("chat-box");

// Function to append a message
function appendMessage(text, className) {
  const msg = document.createElement("div");
  msg.classList.add("message", className);
  msg.textContent = text;
  chatBox.appendChild(msg);
  chatBox.scrollTop = chatBox.scrollHeight;
}

// Function to show initial options
function showOptions() {
  appendMessage("Hello Buddy, how may I assist you today?\nKindly choose from one of the following categories to get started.", "ai");

  const buttonContainer = document.createElement("div");
  buttonContainer.id = "option-buttons";

  const options = [
    { text: "1. Admission Enquiry", value: 1 },
    { text: "2. Placement", value: 2 },
    { text: "3. Research", value: 3 },
    { text: "4. Contact Details", value: 4 },
    { text: "5. Show All Programs", value: 5 }
  ];

  options.forEach(option => {
    const button = document.createElement("button");
    button.className = "chat-button";
    button.textContent = option.text;
    button.onclick = () => handleUserChoice(option.value);
    buttonContainer.appendChild(button);
  });

  chatBox.appendChild(buttonContainer);
  chatBox.scrollTop = chatBox.scrollHeight;
}

// Handle user choice
function handleUserChoice(choice) {
  switch (choice) {
    case 1:
      appendMessage("1. Admission Enquiry", "user");
      showAdmissionForm();
      break;
    case 2:
      appendMessage("2. Placement", "user");
      window.open("https://geu.ac.in/placements/", "_blank");
      appendMessage("Opening Placement page...", "ai");
      break;
    case 3:
      appendMessage("3. Research", "user");
      window.open("https://geu.ac.in/research/", "_blank");
      appendMessage("Opening Research page...", "ai");
      break;
    case 4:
      appendMessage("4. Contact Details", "user");
      showContactDetails();
      break;
    case 5:
      appendMessage("5. Show All Programs", "user");
      window.open("https://geu.ac.in/academics/", "_blank");
      appendMessage("Opening Programs page...", "ai");
      break;
    default:
      appendMessage("⚠️ Please select a valid option.", "ai");
  }
}

// Show Admission Form
function showAdmissionForm() {
  const formContainer = document.createElement("div");
  formContainer.className = "message ai";

  formContainer.innerHTML = `
  <form id="admission-form" style="display: flex; flex-direction: column; gap: 10px;">
    <label>Program Type*<br/>
      <input type="text" name="programType" required/>
    </label>

    <label>Name*<br/>
      <input type="text" name="name" required/>
    </label>

    <label>Phone*<br/>
      <input type="tel" name="phone" required/>
    </label>

    <label>Email*<br/>
      <input type="email" name="email" required/>
    </label>

    <label>10th % or Grade*<br/>
      <input type="text" name="tenth" required/>
    </label>

    <label>12th % or Grade*<br/>
      <input type="text" name="twelfth" required/>
    </label>

    <label>Preferred Campus*<br/>
      <input type="text" name="campus" required/>
    </label>

    <div style="display: flex; gap: 10px; margin-top: 10px;">
      <button type="reset" class="chat-button">Reset</button>
      <button type="submit" class="chat-button">Submit</button>
    </div>
  </form>
`;

  chatBox.appendChild(formContainer);
  chatBox.scrollTop = chatBox.scrollHeight;

  // Handle form submission
  document.getElementById("admission-form").onsubmit = (e) => {
    e.preventDefault();
    appendMessage("Thank you for your Admission Enquiry! We will get in touch soon.", "ai");
    e.target.reset();
  };
}

// Show Contact Details
// Show Contact Details with Buttons
function showContactDetails() {
    appendMessage("Please choose a contact option:", "ai");

    const contactOptions = document.createElement("div");
    contactOptions.id = "contact-options";

    const contacts = [
      { text: "Toll-Free ", detail: "Toll-Free : 1800 270 1280\nEmail : enquiry@geu.ac.in" },
      { text: "Student Helpline ", detail: "Student Helpline: +91 90686 79474\nEmail : enquiry@geu.ac.in" }
    ];

    contacts.forEach(contact => {
      const button = document.createElement("button");
      button.className = "chat-button";
      button.textContent = contact.text;
      button.onclick = () => {
        appendMessage(contact.detail, "ai");
        document.getElementById("contact-options").remove(); // remove the buttons after selection
      };
      contactOptions.appendChild(button);
    });

    chatBox.appendChild(contactOptions);
    chatBox.scrollTop = chatBox.scrollHeight;
  }


// Sending user typed messages
function sendUserMessage() {
  const input = document.getElementById("user-input");
  const text = input.value.trim();
  if (text === "") return;
  appendMessage(text, "user");
  input.value = "";

  appendMessage("Kindly select from available options.", "ai");
}

// Enter key handler
function handleKey(event) {
  if (event.key === "Enter") {
    event.preventDefault();
    sendUserMessage();
  }
}

// Start the chatbot when window loads
window.onload = () => {
  showOptions();
};
