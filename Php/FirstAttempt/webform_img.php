<?php
/*  PHPlot web form example - image generation

This draws the plot image for webform.php
It expects the following parameters:
'deposit' = Amount deposited per month. Must be >= 0.
'intrate' = Interest rate as a percentage (e.g. 4 means 4% or 0.04)
'w', 'h' = image width and height. (Must be between 100 and 5000)
*/
require_once 'phplot.php';

# Check for parameters supplied to this web page.
# Parameters must be checked here, even though the calling script checked them,
# because there is nothing stopping someone from calling this script
# directly with arbitrary parameters.
# Parameter values are stored in the param[] array (valid or not).
# If the parameters are valid, return True, else return False.
function check_form_params(&$param)
{
    $valid = True;
    $depost = 0;
    $intrate = 0;
    
    if (!isset($_GET['deposit']) || !is_numeric($_GET['deposit'])
        || ($deposit = floatval($_GET['deposit'])) < 0)
        $valid = False;
        
        if (!isset($_GET['intrate']) || !is_numeric($_GET['intrate'])
            || ($intrate = floatval($_GET['intrate'])) < 0 || $intrate > 100)
            $valid = False;
            
            # If width and height are missing or invalid, just use something reasonable.
            if (empty($_GET['w']) || !is_numeric($_GET['w'])
                || ($w = intval($_GET['w'])) < 100 || $w > 5000)
                $w = 1024;
                if (empty($_GET['h']) || !is_numeric($_GET['h'])
                    || ($h = intval($_GET['h'])) < 100 || $h > 5000)
                    $h = 768;
                    
                    $param = compact('deposit', 'intrate', 'h', 'w');
                    return $valid;
}

# Calculate the data for the plot:
# This is only called if the parameters are valid.
# The calculation is simple. Each month, two points are calculated: the
# cumulative deposts (balance without interest), and balance with interest.
# At time 0 the balance is 0. At the start of each month, 1/12th of
# the annual interest rate is applied to the balance, and then the deposit
# is added, and that is reported as the balance.
# We calculate for a fixed amount of 120 months (10 years).
function calculate_data($param, &$data)
{
    $deposit = $param['deposit'];
    $monthly_intrate = 1.0 + $param['intrate'] / 100.0 / 12.0;
    $balance_without_interest = 0;
    $balance = 0;
    $data = array(array('', 0, 0, 0)); // Starting point
    for ($month = 1; $month <= 120; $month++) {
        $balance_without_interest += $deposit;
        $balance = $balance * $monthly_intrate + $deposit;
        $data[] = array('', $month, $balance_without_interest, $balance);
    }
}

# Draw the graph:
function draw_graph($valid_params, $param, $data)
{
    extract($param);
    
    $plot = new PHPlot($w, $h);
    $plot->SetTitle('Savings with Interest');
    $plot->SetDataType('data-data');
    # Don't set data values if parameters were not valid. This will result
    # in PHPlot making an image with an error message.
    if ($valid_params) {
        $plot->SetDataValues($data);
    }
    $plot->SetLegend(array('Deposits only', 'Deposits with Interest'));
    $plot->SetLegendPixels(100, 50); // Move legend to upper left
    $plot->SetXTitle('Month');
    $plot->SetXTickIncrement(12);
    $plot->SetYTitle('Balance');
    $plot->SetYLabelType('data', 2);
    $plot->SetDrawXGrid(True);
    $plot->SetPlotType('lines');
    $plot->DrawGraph();
}

# This is our main processing code.
$valid_params = check_form_params($param);
if ($valid_params) calculate_data($param, $data);
draw_graph($valid_params, $param, $data);






