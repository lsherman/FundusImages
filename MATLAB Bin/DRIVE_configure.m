%This file sets the path for configuration of DRclassify


% Evaluation files
roi.evalfiles= 'Data\training\drive_train.txt';
roi.evalfilepath = {
'Data\training\images\'; ...
};
roi.maskpath={'Data\training\mask\';};
roi.actfile = 'Data\training\drive_train_act.txt';
roi.actpath={'Data\training\1st_manual\';};

%%----------------------------------------------------------------------
%% Now for the test images
roi.testfiles= 'Data\test\drive_test.txt';
roi.testfilepath = {
'Data\test\images\'; ...
};
roi.maskpath={'Data\training\mask\';};
roi.testactfile = 'Data\test\drive_test_act.txt';
roi.testactpath={'Data\test\1st_manual\';};
