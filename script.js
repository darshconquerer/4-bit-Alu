const byId = id => document.getElementById(id);
const OPERATION = {add: 0, subtract: 1, multiply: 2};

const BIT_COUNT_BY_OPERATION = [5, 4, 8];

let currentOperation = OPERATION.add;
let aluEngine = null;

createALU().then(module => {
    aluEngine = {
        getResultBits: module.cwrap("alu_bits", "number", ["number", "number", "number"]),
        getResultValue: module.cwrap("alu_value", "number", ["number", "number", "number"]),
        getBorrow: module.cwrap("alu_borrow", "number", ["number", "number"])
    };
    updateDisplay();
});
function parseInputNumber(text){
    const isWholeNumber = /^\d{1,2}$/.test(text.trim());
    return isWholeNumber && Number(text) <= 15 ? Number(text): null;
}
function toFourBitString(number){
    return number.toString(2).padStart(4, "0");
}
function renderSmileys(resultBits, bitLabels){
    byId("smileyRow").innerHTML = resultBits.map((bit, i) =>
    `<div class = "sm ${bit ? "on": "off"}"><span>${bit ? "😄" : "😐"}</span>${bitLabels[i]}</div>`
    ).join("");
}
function updateDisplay(){
    byId("errorMessage").textContent = "";
    const numberA = parseInputNumber(byId("numberAInput").value);
    const numberB = parseInputNumber(byId("numberBInput").value);
    byId("numberABinary").textContent = numberA === null ? "": toFourBitString(numberA);
    byId("numberBBinary").textContent = numberB === null ? "": toFourBitString(numberB);
    if(numberA === null || numberB === null){
        byId("errorMessage").textContent = "Enter whole numbers from 0 to 15 in both boxes.";
        byId("smileyRow").innerHTML = "";
        byId("resultInteger").textContent = "_";
        byId("resultBinary").textContent = "_";
        byId("resultNote").textContent = "";
        return;
    }
    if(!aluEngine){
        byId("resultNote").textContent = "Loading the C++ engine.....";
        return;
    }
    const bitCount = BIT_COUNT_BY_OPERATION[currentOperation];
    const packedBits = aluEngine.getResultBits(currentOperation, numberA, numberB);
    const resultValue = aluEngine.getResultValue(currentOperation, numberA, numberB);
    const resultBits = Array.from({length: bitCount}, (_, i) => (packedBits >> i) & 1);
    const bitLabels = resultBits.map((_, i) => "bit " + i);
    let noteText = "";
    let showWarning = false;

    if(currentOperation === OPERATION.add){
        bitLabels[4] = "carry";
        byId("resultTitle").textContent = "Sum";
        noteText = "The carry-out is the fifth bit, so every sum fits. ";
    }
    else if(currentOperation === OPERATION.subtract){
        byId("resultTitle").textContent = "Difference";
        showWarning = aluEngine.getBorrow(numberA, numberB) === 1;
        noteText = showWarning ? "Borrow: A is smaller than B, so the result is negative. The bits are its 2's complement pattern." : "No borrow: A is at least B";
    }
    else{
        byId("resultTitle").textContent = "Product";
        noteText = "The product uses up to 8 bits (15 * 15 = 225), so every result fits.";
    }
    renderSmileys(resultBits, bitLabels);
    byId("resultInteger").textContent = resultValue;
    byId("resultBinary").textContent = [...resultBits].reverse().join("");
    byId("resultNote").textContent = noteText;
    byId("resultNote").className = "note" + (showWarning ? " warn" : "");
}
document.querySelectorAll(".ops button").forEach(button =>{
    button.onclick = () => {
        currentOperation = OPERATION[button.dataset.operation];
        document.querySelectorAll(".ops button").forEach(other => 
            other.setAttribute("aria-pressed", other === button));
        updateDisplay();
    };
});
["numberAInput", "numberBInput"].forEach(inputId => 
    byId(inputId).addEventListener("input", updateDisplay));

document.querySelectorAll(".scroll").forEach(imageBox =>{
    imageBox.onclick = () => imageBox.classList.toggle("zoom");
});
function showCurrentPage(){
    const isBackendPage = location.hash === "#backend";
    byId("calculatorPage").hidden = isBackendPage;
    byId("backendPage").hidden = !isBackendPage;
    window.scrollTo(0, 0);
}
window.addEventListener("hashchange", showCurrentPage);
showCurrentPage();
updateDisplay();