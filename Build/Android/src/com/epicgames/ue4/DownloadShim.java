package com.epicgames.ue4;

import com.pyritesoftworks.SpaceRTS.OBBDownloaderService;
import com.pyritesoftworks.SpaceRTS.DownloaderActivity;


public class DownloadShim
{
	public static OBBDownloaderService DownloaderService;
	public static DownloaderActivity DownloadActivity;
	public static Class<DownloaderActivity> GetDownloaderType() { return DownloaderActivity.class; }
}

