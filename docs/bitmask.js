const maskInputs = [
    "hmr_retailxp",
    "hmr_selfcrafted",
    "hmr_int_auction",
    "hmr_int_gbank"
];

function generateMask() 
{
    let bitmask = 0;
    let i = 1; // Start at 1 because of an internal enum we are skipping. HARDMODE_RESTRICT_NONE

    for(let maskIndex in maskInputs)
    {
        let maskInput = maskInputs[maskIndex];

        let maskInputElement = document.getElementById(maskInput);

        if(maskInputElement.checked)
        {
            console.log(typeof maskIndex);
            bitmask += 1 << i; 
        }

        i += 1;
    }

    let bResult = document.getElementById("bitmask-result");
    bResult.innerHTML = "Result: " + bitmask;
}