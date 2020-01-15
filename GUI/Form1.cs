using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Media;
using System.Runtime.InteropServices;
using System.IO;
using NAudio.Wave;
using NAudio.Wave.SampleProviders;


namespace DjBotGui
{ 
    public partial class Form1 : Form
    {

        #region Dll stuff
        const string BOT_DLL_ADDR = @"E:\Visual Studio 2019 Projects\DjBot\AiDj\Debug\AiDj.dll";/*
        The address to the dll containing the code for the bot*/

        [DllImport(BOT_DLL_ADDR, CallingConvention = CallingConvention.Cdecl)]
        static extern void Initialize(int numOfSamples, float exploratoryProbability,
        float[] sampleDuration, bool newSampleSet);

        [DllImport(BOT_DLL_ADDR, CallingConvention = CallingConvention.Cdecl)]
        static extern void GetTrack(int[] trackBuffer, float songDuration, 
            int numOfSamples);

        [DllImport(BOT_DLL_ADDR, CallingConvention = CallingConvention.Cdecl)]
        static extern void GiveFeedBack(float rating);

        [DllImport(BOT_DLL_ADDR, CallingConvention = CallingConvention.Cdecl)]
        static extern void Quit();

        [DllImport(BOT_DLL_ADDR, CallingConvention = CallingConvention.Cdecl)]
        static extern int TotalTracksCreated();

        const string NET_DLL_ADDR = @"E:\Visual Studio 2019 Projects\DjBot\AiDj\Debug\SingleHiddenLayerNeuralNet.dll";
        /*The address of the dll containing the code for the judge neural net*/
        [DllImport(NET_DLL_ADDR, CallingConvention = CallingConvention.Cdecl)]
        static extern void InitializeNet(int[] netStruct, double momentumFactor, 
            double learningRate, char activationFunc);
        [DllImport(NET_DLL_ADDR, CallingConvention = CallingConvention.Cdecl)]
        static extern void GetOutput(double[] inputs, double[] outputBuffer);
        [DllImport(NET_DLL_ADDR, CallingConvention = CallingConvention.Cdecl)]
        static extern void TrainNetwork(double[] inputs, double[] expectedOutputs, 
            int dataSetSize, int epochs, double errorChangeSensitivity);

        #endregion
         
        #region Variables
        //Main Variables
        List<string> samplesList = new List<string>(); /*A list containing the address to the samples*/
        float minDuration; /*The duration of the shortest sample*/
        float songDuration = 3000.0f; /*The duration of the required track (in ms)*/
        int numOfSamples = 10; /*The number of samples in the required track*/
        List<List<int>> tracks = new List<List<int>>(); /*A list of tracks as training
        data for the neural network*/
        List<float> trackRatings = new List<float>(); /*The ratings given to the training
        tracks*/

        //GUI Elements
        Button btn_generateTrack; //The button to generate music track from samples
        TrackBar tkb_ratingSlider; //A trackbar which acts as the slider for rating the track
        Label lbl_rating; //A label which shows the rating 
        Button btn_confirmRating; //Gives the rating to the bot
        Label lbl_tracksCreated; //A label which displays the number of tracks created
        Label lbl_currentSample; //The sample being currently played

        //Enums
        enum ActivationFunctions { Sigmoid = 0, Tanh = 1, ReLu = 2 };

        #endregion

        #region Class Methods
        public Form1()
        {
            InitializeComponent();

            //Initializing the program
            InitializeProgram();

            //Initializing the GUI
            InitializeGUI();
        }

        #endregion

        #region Main Functions
        void InitializeProgram() //Initializes the program
        {
            //Loading the samples
            LoadSampleData();

            //Initializing the bot
            Initialize(samplesList.Count, 0.8f, GetSampleDurations(), true);

            //Initializing the judge neural network
            int[] netStruct = { numOfSamples, 3, 1};
            InitializeNet(netStruct, 0.9, 0.5, (char)ActivationFunctions.Sigmoid);
        }

        void InitializeGUI() //Initializes the gui elements
        {
            //Initializing the form
            InitializeForm();

            //Initilizing button to generate track
            InitializeBtn_generateTrack();

            //Initializing the input ui elements
            InitializeInputUiElements();

            //Initializing the track count label
            InitializeTrackInfoUiElements();
        }

        #endregion

        #region Helper Functions
        void LoadSampleData() //Loads the samples
        {
            DirectoryInfo sampleDirectory = new DirectoryInfo(
                Directory.GetCurrentDirectory() + "\\Samples"); /*The directory
            containing the samples*/

            //Getting the samples from the directory
            GetMp3Samples(sampleDirectory);
            GetWavSamples(sampleDirectory);

        }

        void GetMp3Samples(DirectoryInfo directory) //Loads the mp3 samples
        {
            //Getting the samples
            FileInfo[] samples = directory.GetFiles("*.mp3", SearchOption.AllDirectories);

            //Adding the mp3 samples to the list of samples
            foreach (FileInfo sample in samples)
            {
                samplesList.Add(sample.FullName);
            }

        }

        void GetWavSamples(DirectoryInfo directory) //Loads the Wav samples
        {
            //Getting the samples
            FileInfo[] samples = directory.GetFiles("*.wav", SearchOption.AllDirectories);

            //Adding the mp3 samples to the list of samples
            foreach (FileInfo sample in samples)
            {
                samplesList.Add(sample.FullName);
            }

        }

        float[] GetSampleDurations() //Gets the durations of the samples
        {
            float[] durations = new float[samplesList.Count];

            //Getting the duration for the first sample
            AudioFileReader audioFile = new AudioFileReader(samplesList[0]);
            durations[0] = (float)audioFile.TotalTime.TotalMilliseconds;
            minDuration = durations[0];

            for (int a = 1; a < samplesList.Count; ++a)
            {
                audioFile = new AudioFileReader(samplesList[a]);
                durations[a] = (float)audioFile.TotalTime.TotalMilliseconds;
                if (durations[a] < minDuration)
                    minDuration = durations[a];
            }

            //Disposing the audio stream after use
            audioFile.Dispose();

            return durations;
        }

        void InitializeForm() //Initializes the form
        {
            this.Size = new Size(1280, 720);        
        }

        void InitializeBtn_generateTrack() //Initializes the button used to generate the track
        {
            btn_generateTrack = new Button();
            btn_generateTrack.Size = new Size(100, 100);
            btn_generateTrack.Location = new Point(this.Width / 2, this.Height / 2);
            btn_generateTrack.Text = "Generate";
            btn_generateTrack.Click += GenerateTrack;
            this.Controls.Add(btn_generateTrack);
        }

        void InitializeInputUiElements() /*Initializes the ui used to get user input*/
        {
            //Initialzing the track bar
            tkb_ratingSlider = new TrackBar();
            tkb_ratingSlider.Size = new Size(200, 100);
            tkb_ratingSlider.Location = new Point(btn_generateTrack.Location.X,
                btn_generateTrack.Location.Y - (tkb_ratingSlider.Size.Height * 2));
            tkb_ratingSlider.Maximum = 10;
            tkb_ratingSlider.TickFrequency = 1;
            tkb_ratingSlider.ValueChanged += RatingChanged;
            /*tkb_ratingSlider.Visible = false;
            tkb_ratingSlider.Enabled = false;*/
            this.Controls.Add(tkb_ratingSlider);

            lbl_rating = new Label();
            lbl_rating.AutoSize = true;
            lbl_rating.BorderStyle = BorderStyle.FixedSingle;
            lbl_rating.Text = "0.0";
            lbl_rating.Location = new Point(tkb_ratingSlider.Location.X,
             tkb_ratingSlider.Location.Y - (lbl_rating.Size.Height * 2));
            this.Controls.Add(lbl_rating);

            btn_confirmRating = new Button();
            btn_confirmRating.Size = new Size(100, 100);
            btn_confirmRating.Location = new Point(btn_generateTrack.Location.X +
                btn_generateTrack.Width + 20, btn_generateTrack.Location.Y);
            btn_confirmRating.Text = "Confirm";
            btn_confirmRating.Click += ConfirmRating;
            this.Controls.Add(btn_confirmRating);
        }

        void InitializeTrackInfoUiElements() /*Initializes the ui elements which give more
         info about the track*/
        {
            //Initializing the label which displays the track counter
            lbl_tracksCreated = new Label();
            lbl_tracksCreated.AutoSize = true;
            lbl_tracksCreated.Text = "Tracks Created : " + TotalTracksCreated();
            lbl_tracksCreated.Location = new Point((this.Width / 2) -
                (lbl_tracksCreated.Width / 2), 10);
            this.Controls.Add(lbl_tracksCreated);

            //Initializing the label which displays the sample being played
            lbl_currentSample = new Label();
            lbl_currentSample.AutoSize = true;
            lbl_currentSample.Text = "Current sample :- -1";
            lbl_currentSample.Location = new Point(lbl_tracksCreated.Location.X,
                lbl_tracksCreated.Location.Y + (lbl_tracksCreated.Height * 2));
            this.Controls.Add(lbl_currentSample);

        }

        double[] ReturnTrainingInputs()
        {
            double[] inputs = new double[tracks.Count * numOfSamples];

            for(int a = 0; a < tracks.Count; ++a)
            {
                for(int b = 0; b < numOfSamples; ++b)
                {
                    if (b >= tracks[a].Count)
                    {
                        inputs[(a * numOfSamples) + b] = -1;
                        continue;
                    }
                    inputs[(a * numOfSamples) + b] = tracks[a][b];
                }
            }

            return inputs;
        }

        double[] ReturnTrainingOutputs()
        {
            double[] outputs = new double[trackRatings.Count];

            for(int a = 0; a < trackRatings.Count; ++a)
            {
                outputs[a] = trackRatings[a];
            }

            return outputs;
        }

        #endregion

        #region Event Handlers
        void GenerateTrack(object sender, EventArgs e) //Gets a track from the bot
        {

            int totalTracksCreated = TotalTracksCreated();

            if (totalTracksCreated <= numOfSamples * 3)
            {
                int[] track = new int[numOfSamples]; /*A buffer for 
                storing the sample order i.e track*/

                //Getting the track from the bot
                GetTrack(track, 0.0f, numOfSamples);

                //Playing the track
                for (int a = 0; a < numOfSamples; ++a)
                {
                    WaveOutEvent waveOutEvent = new WaveOutEvent(); //Plays the audio track
                    AudioFileReader sampleFile = new AudioFileReader(samplesList[track[a]]);
                    //A stream to the sample audio file

                    //Playing the sample
                    waveOutEvent.Init(sampleFile);
                    waveOutEvent.Play();

                    //Waiting for the playback to finish
                    while (waveOutEvent.PlaybackState != PlaybackState.Stopped)
                    {
                    }
                }

                //Updating the track counter label
                totalTracksCreated = TotalTracksCreated();

                List<int> trackSamples = new List<int>();
                for (int a = 0; a < track.Length; ++a)
                {
                    if (track[a] > samplesList.Count)
                        break;
                    trackSamples.Add(track[a]);
                }

                tracks.Add(trackSamples);
            }
            else if (totalTracksCreated > (numOfSamples * 3) && totalTracksCreated < (numOfSamples * 3) + 100)
            {
                for (int i = 0; i < 100; ++i)
                {
                    int[] track = new int[numOfSamples]; /*A buffer for 
                    storing the sample order i.e track*/

                    //Getting the track from the bot
                    GetTrack(track, 0.0f, numOfSamples);

                    List<int> trackSamples = new List<int>();
                    for (int a = 0; a < track.Length; ++a)
                    {
                        if (track[a] > samplesList.Count)
                            break;
                        trackSamples.Add(track[a]);
                    }
                    tracks.Add(trackSamples);

                    double[] outputs = new double[1];
                    GetOutput(ReturnTrainingInputs(), outputs);

                    GiveFeedBack((float)outputs[0]);

                    tracks.Clear();
                }
            }
            else
            {
                int[] track = new int[numOfSamples]; /*A buffer for 
                storing the sample order i.e track*/

                //Getting the track from the bot
                GetTrack(track, 0.0f, numOfSamples);

                //Playing the track
                for (int a = 0; a < numOfSamples; ++a)
                {
                    WaveOutEvent waveOutEvent = new WaveOutEvent(); //Plays the audio track
                    AudioFileReader sampleFile = new AudioFileReader(samplesList[track[a]]);
                    //A stream to the sample audio file

                    //Playing the sample
                    waveOutEvent.Init(sampleFile);
                    waveOutEvent.Play();

                    //Waiting for the playback to finish
                    while (waveOutEvent.PlaybackState != PlaybackState.Stopped)
                    {
                    }
                }
            }
            lbl_tracksCreated.Text = "Tracks Created : " + totalTracksCreated;
        }

        void RatingChanged(object sender, EventArgs e)
        {
            lbl_rating.Text = tkb_ratingSlider.Value.ToString();
        }

        void ConfirmRating(object sender, EventArgs e)
        {
            int totalTracksCreated = TotalTracksCreated();

            if (totalTracksCreated < numOfSamples * 3)
            {
                float rating = (float)tkb_ratingSlider.Value;
                trackRatings.Add(rating);
                GiveFeedBack(rating);
            }
            else if (totalTracksCreated == numOfSamples * 3)
            {
                float rating = (float)tkb_ratingSlider.Value;
                trackRatings.Add(rating);
                GiveFeedBack(rating);

                TrainNetwork(ReturnTrainingInputs(), ReturnTrainingOutputs(),
                    numOfSamples, 0, 0.01);

                tracks.Clear();
                trackRatings.Clear();
            }

        }

        #endregion

    }
}
