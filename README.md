# SubPar

## Automatic subtitle downloader
This app is supposed to find the best possible subtitle for your movie file.
It uses the OpenSubtitles api, so you will need to provide your own api key to make it work.

## OpenSubtitles API key
Obtaining a key is really easy, go to https://www.opensubtitles.com and login or register for a new account.
On your profile go to api consumers and create a new one by clicking on "New Consumer"
You'll need to provide a name and a description, I don't think it matters much.
The other option there is to mark your consumer is under development, I suggest you check it if you're gonna play with the api code, otherwise you may end up using your quota really fast, but remember to uncheck it if you release your own executable.
Put the key in the file **opensubtitlesapi.h**

## License
This code is release under the GPLv3 license, you can read more here https://www.gnu.org/licenses/gpl-3.0.html
Or a quick guide here https://www.gnu.org/licenses/quick-guide-gplv3.html

This means that
* you can use this software for any purpose
* you can change it as much as you want
* you can share it with anyone
* you can share the changes you make

But, any derivate work must also be released under the GPLv3 license
