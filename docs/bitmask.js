const maskInputs = [
    "hmr_retailxp",
    "hmr_selfcrafted",
    "hmr_int_auction",
    "hmr_int_gbank",
    "hmr_int_msend",
    "hmr_int_mrec",
    "hmr_int_trade",
    "hmr_permadeath",
    "hmr_hide_who",
    "hmr_hide_friends",
    "hmr_hide_guild",
    "hmr_int_lfg",
    "hmr_int_grp",
    "hmr_int_crossplay",
    "hmr_unlucky",
    "hmr_pacifist",
	"hmr_int_grp_range",
	"hmr_int_talent",
	"hmr_smallfish"
];

var modeName = "";
var modeDesc = "";
var modeColor = "#ffffff";
var modeMask = 0;
var modeEnabled = false;

function openColorPicker()
{
    let colorPickerElement = document.getElementById("mode-color");
    let colorPickerButtonElement = document.getElementById("mode-color-button");
    colorPickerElement.addEventListener("change", () => {
        modeColor = colorPickerElement.value;
        colorPickerButtonElement.style.backgroundColor = modeColor;
    });
    colorPickerElement.click();
}

function loadMask()
{
    let editMaskInputElement = document.getElementById("bitmask-edit");
    if(!editMaskInputElement)
    {
        console.log("An unexpected error occurred when trying to get element bitmask-edit.");
        return;
    }

    let editMaskInputValue = editMaskInputElement.value;
    if(!editMaskInputValue)
    {
        return;
    }

    let i = 1; // Start at 1 because of an internal enum we are skipping. HARDMODE_RESTRICT_NONE

    for(let maskIndex in maskInputs)
    {
        let maskInput = maskInputs[maskIndex];

        let maskInputElement = document.getElementById(maskInput);

        if(editMaskInputValue & (1 << i))
        {
            maskInputElement.checked = true;
        }
        else
        {
            maskInputElement.checked = false;
        }

        i += 1;
    }
}

function generateSQL()
{
    generateMask();
    document.getElementById("bitmask-edit").value = modeMask;
    modeName = document.getElementById("mode-name").value;
    modeDesc = document.getElementById("mode-desc").value;
    modeEnabled = document.getElementById("hm-enabled").checked;
    let strippedColor = modeColor.substring(1);
    let modeSqlElement = document.getElementById("sql-result");
    modeSqlElement.innerHTML = `INSERT INTO hardmode_modes (name, description, restrictions, enabled) VALUES ('|cff${strippedColor}${modeName}|r', '${modeDesc}', ${modeMask}, ${modeEnabled ? 1 : 0});`;
}

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
            bitmask += 1 << i; 
        }

        i += 1;
    }

    modeMask = bitmask;
}