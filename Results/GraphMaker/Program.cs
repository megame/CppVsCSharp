using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Diagnostics;
using System.Drawing;

namespace GraphMaker
{
	class Program
	{
		static void Main(string[] args)
		{
			var platforms = new List<string>();
			var data = new Dictionary<string,List<float>>();
			
			ReadFiles(args, platforms, data);

			using (TextWriter tw = new StreamWriter("test.html"))
			{
				tw.WriteLine("<html><head></head><body>");
				tw.WriteLine("<h3>Charts</h3>");

				var p = new Program(data, platforms, tw);
				p.MakeCharts();

				tw.WriteLine("</body></html>");
			}

			Process.Start(new FileInfo("test.html").FullName);
		}

		#region File reader

		private static void ReadFiles(string[] args, List<string> platforms, Dictionary<string,List<float>> data)
		{
			if (args.Length == 0)
			{
				string folder = Environment.CurrentDirectory;

				for (int i = 0; i < 5; i++) {
					ReadFiles(data, platforms, Directory.GetFiles(folder, "*.csv"));
					folder = Path.Combine(folder, "..");
				}
			} else
				ReadFiles(data, platforms, args);

			Console.WriteLine(data.Count);
		}

		private static void ReadFiles(Dictionary<string,List<float>> data, List<string> platforms, string[] args)
		{
			// This is not my best code.
			
			foreach (string path in args)
			{
				StreamReader reader = File.OpenText(path);
				// Ignore the first line
				string line = reader.ReadLine();

				int platformIndex = platforms.Count;
				platforms.Add(Path.GetFileNameWithoutExtension(path));
				foreach (var list in data.Values)
					list.Add(float.NaN);
				
				while ((line = reader.ReadLine()) != null)
				{
					string[] fields = line.Split(',');
					string name = fields[0].TrimEnd();
					float average = float.Parse(fields[2]);

					name = name.Replace("Dictionary", "hashtable")
					           .Replace("hash_map", "hashtable")
								  .Replace('¸', ',');

					if (!data.ContainsKey(name))
					{
						var l = new float[platforms.Count];
						for (int i = 0; i < l.Length; i++)
							l[i] = float.NaN;
						data.Add(name, new List<float>(l));
					}
					data[name][platformIndex] = average;
				}
			}
		}

		#endregion

		#region Code to build Google Chart URLs

		Dictionary<string, List<float>> _data;
		List<string> _platforms;
		TextWriter _tw;

		internal Program(Dictionary<string, List<float>> data, List<string> platforms, TextWriter tw)
		{
			_data = data;
			_platforms = platforms;
			_tw = tw;
		}

		// Google chart API docs: http://code.google.com/apis/chart/image/docs/making_charts.html
		// Bar chart API docs: http://code.google.com/apis/chart/image/docs/gallery/bar_charts.html
		readonly static string
			ChartSize = "&chs=",        // chs=640x468 (Google limits charts to 300,000 pixels!)
			ChartData = "&chd=t:",      // chd=t:1,2,3,4|6,3,4,5|1,2,3,7|3,3,4,6
			                            // Note: data is transposed! "1,2,3,4" are the first bar in each group
			ChartColors = "&chco=",     // chco=40C0FF,38A8E0,FF4444,DD0000
			ChartLegend = "&chdl=",     // chdl=C++ x86|C++ x64|C# x86|C# x64
			ChartAxis = "&chxt=",       // chxt=x,y,x (you can use a duplicate axis for extra labels)
			ChartAxisLabels = "&chxl=", // chxl=-1:|Test A|Test B|Test C|Test D|2:|fast|Time in Seconds|slow
				// Note: "-1" refers to axis #1 (y). If you use "1" then the labels are reversed!
			BarSpacingOptions = "&chbh=", // chbh=a,0,10  (bar size (a=auto), in-group spacing, inter-group spacing)
				// If you want to specify a range, you must use both chxr and chds. chxr scales the LABELS, 
				// chds scales the DATA. Use chds=a to scale the data AND labels automatically.
			ChartRangeXR = "&chxr=",    // chxr=0,0,10 (axis#,min,max; DOES NOT ACTUALLY SCALE THE BARS!)
			ChartRangeDS = "&chds=",    // chds=0,10 (x min, xmax, y min, y max; DOES NOT CHANGE THE LABELS!)
				// chg ignores the data range, so it's best to think of it as a pair of percentages.
				// Of course, this means if you want a grid, you probably can't use "chds=a".
			ChartGridSpacing = "&chg=", // chg=20,0 (x %, y %, dash px, space px, x offs, y offs)
			ChartDataLabels = "&chm=",  // chm=N(f0x),000000,1
				// chm=<marker_type>,<color>,<series_index>,<opt_which_points>,<size>,<opt_z_order>,<opt_placement>
				// see http://code.google.com/apis/chart/image/docs/gallery/bar_charts.html#gcharts_data_point_labels
			ChartTitle = "&chtt=";      // chtt=Hello, world!

		static string BarChartBaseUrl(string type)
		{
			return "http://chart.apis.google.com/chart?cht=" + (type ?? "bhg");
		}

		class Row
		{
			public string Name;
			public float[] Values; // NaN if not applicable
		}
		class DataSet
		{
			public List<string> SeriesNames = new List<string>();
			public List<Row> Rows = new List<Row>();
			
			public StringBuilder ChartDataString(out float min, out float max)
			{
				bool first = true;
				min = 0;
				max = 0;

				StringBuilder sb = new StringBuilder(ChartData);
				for (int s = 0; s < SeriesNames.Count; s++)
				{
					if (s != 0)
						sb.Append('|');
					for (int r = 0; r < Rows.Count; r++)
					{
						var row = Rows[r];
						if (r != 0)
							sb.Append(',');
						float value = s < row.Values.Length ? row.Values[s] : float.NaN;
						if (s < row.Values.Length && !float.IsNaN(row.Values[s]))
						{
							sb.Append(value.ToString("0.##"));
							if (first) {
								min = max = value;
								first = false;
							} else {
								min = Math.Min(min, value);
								max = Math.Max(max, value);
							}
						}
						else
							sb.Append('_');
					}
				}
				return sb;
			}
			
			public string ChartDataConfigured(Func<string, Color> colorPicker, float maxMax, string xAxisExtra, int maxGridLines)
			{
				float min, max;
				StringBuilder sb = ChartDataString(out min, out max);

				// Normally the graph should start at zero.
				if (min > 0 && max > 0)
					min = 0;
				// If the data exceeds the maximum, truncate the graph by setting the data range
				// e.g. ChartRangeDS + "min,max" + ChartRangeXR + "0,min,max"
				if (max > maxMax)
				{
					max = maxMax;
					sb.Append(string.Format("{0}{1:0.###},{2:0.###}", ChartRangeDS, min, max));
					sb.Append(string.Format("{0}0,{1:0.###},{2:0.###}", ChartRangeXR, min, max));
				}

				// Add chart gridlines: ChartGridSpacing + "12.5,0"; the numbers are
				// percentages. All params: (x%, y%, dash px, space px, x offs, y offs)
				// First challenge is to choose the spacing between gridlines:
				if (maxGridLines > 1)
				{
					float spacing = 0.00001f, range = max - min;
					for(;;) {
						if (spacing * maxGridLines > range) break;
						spacing *= 2f;
						if (spacing * maxGridLines > range) break;
						spacing *= 2.5f;
						if (spacing * maxGridLines > range) break;
						spacing *= 2f;
					}
					float spacingPercent = range / spacing * 100f;
					float spacingOffset = (min / spacing - (float)Math.Floor(min / spacing)) * spacingPercent;
					sb.Append(ChartGridSpacing);
					sb.Append(string.Format("{0:0.##},0,4,1,{1:0.##},0", spacingPercent, spacingOffset));
				}

				// Add series labels, e.g. ChartLegend + "C++ x86|C++ x64|C# x86|C# x64"
				sb.Append(ChartLegend);
				for (int i = 0; i < SeriesNames.Count; i++)
				{
					if (i != 0)
						sb.Append('|');
					sb.Append(SeriesNames[i]);
				}

				// Add row labels, e.g. ChartAxis + "x,y" + ChartAxisLabels + "-1:|Test A|Test B|Test C|Test D"
				if (xAxisExtra != null) // should be something like "fast|Time in Seconds|slow"
					sb.Append(ChartAxis + "x,y,x" + ChartAxisLabels + "2:|" + xAxisExtra + "|-1:");
				else
					sb.Append(ChartAxis + "x,y" + ChartAxisLabels + "-1:");
				for (int r = 0; r < Rows.Count; r++)
				{
					sb.Append('|');
					sb.Append(Rows[r].Name);
				}

				// Add series colors, e.g. ChartColors + "40C0FF,38A8E0,FF4444,DD0000"
				sb.Append(ChartColors);
				for (int i = 0; i < SeriesNames.Count; i++)
				{
					if (i != 0)
						sb.Append(',');
					sb.Append(ColorToHex(colorPicker(SeriesNames[i])));
				}

				return sb.ToString();
			}

			static char[] ColorToHex(Color color)
			{
				var c = new char[6];
				c[0] = GetHexChar(color.R >> 4);
				c[1] = GetHexChar(color.R & 15);
				c[2] = GetHexChar(color.G >> 4);
				c[3] = GetHexChar(color.G & 15);
				c[4] = GetHexChar(color.B >> 4);
				c[5] = GetHexChar(color.B & 15);
				return c;
			}
			static char GetHexChar(int digit)
			{
				digit &= 15;
				return digit < 10 ? (char)('0' + digit) : (char)(digit + ('A' - 10));
			}
		}

		DataSet GatherData(Predicate<string> platformSelector, bool includeDebug, Predicate<string> rowSelector)
		{
			DataSet results = new DataSet();
			var platformIndexes = new List<int>();
			for (int i = 0; i < _platforms.Count; i++)
			{
				string name = _platforms[i];
				if (name.StartsWith("Results"))
					name = name.Substring("Results".Length);

				if (platformSelector(name) && (includeDebug || !name.Contains("Debug")))
				{
					platformIndexes.Add(i);
					results.SeriesNames.Add(name);
				}
			}

			int platformCount = platformIndexes.Count;

			foreach (var kvp in _data)
			{
				if (rowSelector(kvp.Key)) {
					var row = new Row { Name = kvp.Key, Values = new float[platformCount] };
					for (int i = 0; i < platformCount; i++)
						row.Values[i] = kvp.Value[platformIndexes[i]];
					results.Rows.Add(row);
				}
			}

			return results;
		}

		Color PlatformColorPicker(string platform)
		{
			int hue = platform.Contains("C#") ? 130 : 0;
			int sat = platform.Contains("VC10") || platform.Contains("NET4") ? 255 : platform.Contains("Mono") ? 32 : 160;
			int lum = platform.Contains("x64") ? 192 : platform.Contains("C#") || platform.Contains("SSE2") ? 140 : platform.Contains("NCI") ? 110 : 80;
			return HslToRgb(hue, sat, lum);
		}

		internal void MakeCharts()
		{
			const string XAxisLabel = "fast|Time in Seconds|slow";

			DataSet data;
			data = GatherData(p => p.Contains("C#"), false,
				               r => r.Contains("Matrix multiply") && r.Contains("double") && (r.Contains("Array2D") || !r.Contains("<")));
			RemovePrefix(data, "Matrix multiply: ");
			Prioritize(data, "[n,n]", "Array2D", "[n][n]", "[n*n]");

			_tw.WriteLine(getImageTag(BarChartBaseUrl("bhg")
				+ ChartTitle + "Effect of different array representations on matrix multiplication speed in C#"
				+ ChartDataLabels + "N*f*,F00000,0,-1,11|N*f*,00F000,1,-1,11|N*f*,0000F0,2,-1,11"
				+ ChartSize + "640x350"
				+ BarSpacingOptions + "a,0,10"
				+ data.ChartDataConfigured(PlatformColorPicker, 6.2f, XAxisLabel, 20)));

			_tw.WriteLine(getImageTag(BarChartBaseUrl("bhg")
				+ ChartSize + "640x468"
				+ ChartData + "1,2,3,4|6,3,4,5|1,2,3,7|3,3,4,6"
				+ ChartColors + "40C0FF,38A8E0,FF4444,DD0000"
				+ ChartLegend + "C++ x86|C++ x64|C# x86|C# x64"
				+ ChartAxis + "x,y,x"
				+ ChartAxisLabels + "-1:|Test A|Test B|Test C|Test D|2:|fast|Time in Seconds|slow"
				+ BarSpacingOptions + "a,0,10"
				+ ChartRangeXR + "0,0,8"
				+ ChartRangeDS + "0,8"
				+ ChartGridSpacing + "12.5,0"
				+ ChartDataLabels + "N*f0*,F00000,0,-1,11|N*f0*,00F000,1,-1,11|N*f0*,0000F0,2,-1,11|N*f0*,000000,3,-1,11"
				+ ChartTitle + "Hello, world!"
				));
		}

		private void RemovePrefix(DataSet data, string prefix)
		{
			for (int i = 0; i < data.Rows.Count; i++) {
				Row row = data.Rows[i];
				if (row.Name.StartsWith(prefix))
					row.Name = row.Name.Substring(prefix.Length);
			}
		}

		// Moves rows whose Name contains specified strings to the top. 'substrings'
		// is in priority order, e.g. rows containing substrings[1] are listed after 
		// those that contain substrings[0]. Not designed to be fast.
		private void Prioritize(DataSet data, params string[] substrings)
		{
			int added = 0;
			for (int i = 0; i < substrings.Length; i++)
			{
				for (int r = data.Rows.Count - 1; r >= added; r--)
				{
					var row = data.Rows[r];
					if (row.Name.Contains(substrings[i]))
					{
						data.Rows.RemoveAt(r);
						data.Rows.Insert(added++, row);
						r++;
					}
				}
			}
		}

		static string getImageTag(string url)
		{
			return "<img src=\"" + url.Replace("#","%23").Replace("+","%2B") + "\" />";
		}

		/// <summary>Converts Hue-Saturation-Luminosity to a Color. hue, sat and
		/// lum must range from 0 to 255.</summary>
		/// <remarks>Hues range from 0 to 255 with 0 being pure red, 85 being pure
		/// green and 171 being pure blue.</remarks>
		public static Color HslToRgb(int hue, int sat, int lum)
		{
			int r, g, b;
			int v = (lum < 128) ? (lum * (256 + sat)) >> 8 : (((lum + sat) << 8) - lum * sat) >> 8;
			r = g = b = 0;
			if (v > 0)
			{
				int m;
				int sextant;
				int fract, vsf, mid1, mid2;

				m = lum + lum - v;
				hue *= 6;
				sextant = hue >> 8;
				fract = hue - (sextant << 8);
				vsf = v * fract * (v - m) / v >> 8;
				mid1 = m + vsf;
				mid2 = v - vsf;
				switch (sextant)
				{
					case 0: r = v; g = mid1; b = m; break;
					case 1: r = mid2; g = v; b = m; break;
					case 2: r = m; g = v; b = mid1; break;
					case 3: r = m; g = mid2; b = v; break;
					case 4: r = mid1; g = m; b = v; break;
					case 5: r = v; g = m; b = mid2; break;
				}
			}
			return Color.FromArgb(r, g, b);
		}

		/// <summary>Converts a color to Hue-Saturation-Luminosity representation
		/// with each component in the range 0 to 255.</summary>
		/// <remarks>If the color is monochrome, the hue argument is left 
		/// unchanged. Note: RgbToHsl() came from a different source than HslToRgb(). 
		/// The conversions probably don't match up perfectly.</remarks>
		public static void RgbToHsl(Color c, ref int hue, out int sat, out int lum)
		{
			int r = c.R, g = c.G, b = c.B;
			int min = Math.Min(Math.Min(r, g), b);
			int max = Math.Max(Math.Max(r, g), b);

			if (min != max)
			{
				if (r == max && g >= b)
					hue = (g - b) * 85 / (max - min) / 2 + 0;
				else if (r == max && g < b)
					hue = (g - b) * 85 / (max - min) / 2 + 255;
				else if (g == max)
					hue = (b - r) * 85 / (max - min) / 2 + 85;
				else if (b == max)
					hue = (r - g) * 85 / (max - min) / 2 + 171;
			}

			lum = (min + max) / 2;

			if (min == max)
				sat = 0; // gray
			else if (min + max < 256)
				sat = (max - min) << 8 / (min + max);
			else
				sat = (max - min) << 8 / (512 - min - max);
		}
		
		#endregion
	}
}
