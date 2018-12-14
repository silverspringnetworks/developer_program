/*
Silver Spring Networks, Inc. ("Silver Spring") licenses this software to you and the company (“You” or “Your”) 
a license for this software (“Software”).  
Using the Software constitutes Your acceptance of these terms (“License”).  

License Grant. Subject to Your acceptance and compliance with this License, Silver Spring grants to You, solely for Your own internal business purpose, 
               a non-exclusive, non-transferable license to access and use the Software and the associated user documentation (“Documentation”) 
               for the term and number agreed to be Silver Spring. 

Restrictions. No intellectual property rights are granted under this License and Silver Spring reserves all rights not expressly granted. 
You may not:  
(a) modify or create derivative works of the Software or Documentation; 
(b) assign, transfer, lease or sublicense the Software or Documentation to any third party 
    (other than Your consultants who are bound to written obligations of confidentiality at least as restrictive as those contained in this License); 
and (c) reverse engineer, disassemble, decrypt, extract or otherwise reduce the Software to a human perceivable 
    form or otherwise attempt to determine the source code or algorithms of the Software 
    (unless the foregoing restriction is expressly prohibited by applicable law).
You may not remove or destroy any proprietary, trademark or copyright markings or notices placed on or contained in the Software or Documentation.  
Silver Spring PROVIDES THE SOFTWARE “AS IS” AND MAKES NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY REGARDING OR RELATING TO THE SOFTWARE.  
Silver Spring HEREBY DISCLAIMS ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, 
WHETHER RIGHTS ARISES BY LAW, BY USE AND CUSTOM OR USAGE IN THE TRADE, OR BY COURSE OF DEALING. 
Silver Spring DOES NOT WARRANT THAT THE SOFTWARE OR ANY PORTION THEREOF WILL BE ERROR-FREE.

Termination. This License is effective until terminated. Your rights under this License will terminate automatically without notice if you fail to comply with any of its terms. 
Upon the termination of this License, You must cease all use of the Software and remove it from Your computer.
*/
using System;
using log4net;
using System.Reflection;
using System.Windows.Forms;


namespace HdkClient
{
    /// <summary>
    /// Log data to a text file.
    /// </summary>
    public class FileLogger
    {
        /// <summary>
        /// The granularity of logging.  Debug is the most detailed, FatalError the least.  Off means no logging.
        /// </summary>
        public enum LogLevel
        {
            Debug,
            Info,
            Warning,
            Error,
            FatalError,
            Off
        }

        private static LogLevel m_LogLevel = LogLevel.Debug;
        private static string m_Path = "";
        private static bool m_Enabled = false;
        private static long m_MaxFileSize = 100000;

        private static log4net.Appender.RollingFileAppender m_Appender;
        private static ILog m_Log;
        private static TextBox m_TextBoxLog;

        /// <summary>
        /// Constructor for this class.
        /// Initializes the log4net wrapper.
        /// </summary>
        static FileLogger()
        {
            m_Appender = new log4net.Appender.RollingFileAppender();
            m_Appender.Name = "DebugLog";
            m_Appender.File = System.IO.Path.GetDirectoryName(Assembly.GetCallingAssembly().Location) + System.IO.Path.DirectorySeparatorChar + "DebugLog.txt";
            m_Path = m_Appender.File;
            m_Appender.AppendToFile = true;
            m_Appender.LockingModel = new log4net.Appender.FileAppender.MinimalLock();
            m_Appender.Layout = new log4net.Layout.PatternLayout("%date [%thread] %-5level %logger - %message%newline");
            m_Appender.ImmediateFlush = true;
            m_Appender.Threshold = log4net.Core.Level.Off;
            m_Appender.RollingStyle = log4net.Appender.RollingFileAppender.RollingMode.Size;
            m_Appender.MaxFileSize = m_MaxFileSize;
            try
            {
                m_Appender.ActivateOptions();
            }
            catch (Exception ee)
            {
                string excep = ee.Message;
            }
            log4net.Config.BasicConfigurator.Configure(m_Appender);
            m_Log = log4net.LogManager.GetLogger("DebugLog");
            //foreach (log4net.ILog log in log4net.LogManager.GetCurrentLoggers())
            //{
            //    log4net.Repository.Hierarchy.Logger logger = (log4net.Repository.Hierarchy.Logger)log.Logger;
            //    Debug.WriteLine(String.Format("{0} Parent {1} Level {2} EffectiveLevel {3}<br>", logger.Name, logger.Parent.Name, logger.Level == null ? "<null>" : logger.Level.Name, logger.EffectiveLevel));
            //}
        }

        /// <summary>
        /// Level allows for different levels of granularity in the log.
        /// Higher levels represent more detail.
        /// </summary>
        public static LogLevel Level
        {
            get { return m_LogLevel; }
            set
            {
                m_LogLevel = value;
                string val = Convert.ToString(value);
                log4net.Repository.Hierarchy.Logger logger = (log4net.Repository.Hierarchy.Logger)m_Log.Logger;

                switch (val)
                {
                    case "Debug":
                        logger.Level = log4net.Core.Level.Debug;
                        m_Appender.Threshold = log4net.Core.Level.Debug;
                        break;
                    case "Info":
                        logger.Level = log4net.Core.Level.Info;
                        m_Appender.Threshold = log4net.Core.Level.Info;
                        break;
                    case "Warning":
                        logger.Level = log4net.Core.Level.Warn;
                        m_Appender.Threshold = log4net.Core.Level.Warn;
                        break;
                    case "Error":
                        logger.Level = log4net.Core.Level.Error;
                        m_Appender.Threshold = log4net.Core.Level.Error;
                        break;
                    case "FatalError":
                        logger.Level = log4net.Core.Level.Severe;
                        m_Appender.Threshold = log4net.Core.Level.Severe;
                        break;
                    default:
                        logger.Level = log4net.Core.Level.Off;
                        m_Appender.Threshold = log4net.Core.Level.Off;
                        break;
                }
                m_Appender.ActivateOptions();
            }
        }

        public static TextBox DebugWindow
        {
            get { return m_TextBoxLog; }
            set { m_TextBoxLog = value; }
        }

        /// <summary>
        /// Maximum file size.  Sets the size after which file wraps.
        /// </summary>
        public static long MaxFileSize
        {
            get { return m_MaxFileSize; }
            set
            {
                m_MaxFileSize = value;
                m_Appender.MaxFileSize = m_MaxFileSize;
                m_Appender.ActivateOptions();
            }
        }

        /// <summary>
        /// Enable logging.  True means log.  False means don't log.
        /// </summary>
        public static bool Enabled
        {
            get { return m_Enabled; }
            set { m_Enabled = value; }
        }

        public static void getit()
        {
            string blat = m_Log.Logger.Repository.Name;
        }

        /// <summary>
        /// Path represents the physical path name of the file to write to.
        /// </summary>
        public static string Path
        {
            get { return m_Path; }
            set
            {
                m_Path = value;
                m_Appender.File = m_Path;
                m_Appender.ActivateOptions();
            }
        }

        /// <summary>
        /// Write text to the log file.
        /// </summary>
        /// <param name="data">The text to write to the file</param>
        public static void Write(string data)
        {
            Write(data, LogLevel.Debug);
        }

        /// <summary>
        /// Optionally write text to the log file.
        /// </summary>
        /// <param name="data">The text to write to the file</param>
        /// <param name="level">The granularity of the data to write.  
        /// If property level is not set as high as this parameter then don't write to the log file.</param>
        public static void Write(string data, LogLevel level)
        {
            // If logging is disabled then don't write.
            if (!m_Enabled)
                return;

            // For the TextBox, just write the data out. 
            // The scrolling, wrapping,etc weil be handled 
            // by the TextBox settings prior to this call.
            if (m_TextBoxLog != null)
            {
                // Before appending make sure that we have not reached the 
                // maximum text box length allowed.
                // If we have, clear the textbox and start again.
                if (m_TextBoxLog.InvokeRequired)
                {
                    m_TextBoxLog.BeginInvoke(new MethodInvoker(() => Write(data, level)));
                    //m_TextBoxLog.BeginInvoke(new MethodInvoker(() => m_TextBoxLog.AppendText(data + "\r\n")));
                    return;
                }
                if (m_TextBoxLog.Text.Length + data.Length > m_TextBoxLog.MaxLength)
                {
                    m_TextBoxLog.Text = "";
                }
                m_TextBoxLog.AppendText(data + "\r\n");
            }

            // If level is not currently enabled then don't write.
            switch (level)
            {
                case LogLevel.Debug:
                    m_Log.Debug(data);
                    break;

                case LogLevel.Info:
                    m_Log.Info(data);
                    break;

                case LogLevel.Warning:
                    m_Log.Warn(data);
                    break;

                case LogLevel.Error:
                    m_Log.Error(data);
                    break;

                case LogLevel.FatalError:
                    m_Log.Fatal(data);
                    break;
            }
        }
    }
}
/// <summary>
/// Log data to a text file.
/// </summary>
public class FileLogger
    {
        /// <summary>
        /// The granularity of logging.  Debug is the most detailed, FatalError the least.  Off means no logging.
        /// </summary>
        public enum LogLevel
        {
            Debug,
            Info,
            Warning,
            Error,
            FatalError,
            Off
        }

        private static LogLevel m_LogLevel = LogLevel.Debug;
        private static string m_Path = "";
        private static bool m_Enabled = false;
        private static long m_MaxFileSize = 100000;

        private static log4net.Appender.RollingFileAppender m_Appender;
        private static ILog m_Log;

        /// <summary>
        /// Constructor for this class.
        /// Initializes the log4net wrapper.
        /// </summary>
        static FileLogger()
        {
            m_Appender = new log4net.Appender.RollingFileAppender();
            m_Appender.Name = "DebugLog";
            m_Appender.File = System.IO.Path.GetDirectoryName(Assembly.GetCallingAssembly().Location) + System.IO.Path.DirectorySeparatorChar + "DebugLog.txt";
            m_Appender.AppendToFile = true;
            m_Appender.LockingModel = new log4net.Appender.FileAppender.MinimalLock();
            m_Appender.Layout = new log4net.Layout.PatternLayout("%date [%thread] %-5level %logger - %message%newline");
            m_Appender.ImmediateFlush = true;
            m_Appender.Threshold = log4net.Core.Level.Off;
            m_Appender.RollingStyle = log4net.Appender.RollingFileAppender.RollingMode.Size;
            m_Appender.MaxFileSize = m_MaxFileSize;
            try
            {
                m_Appender.ActivateOptions();
            }
            catch (Exception ee)
            {
                string excep = ee.Message;
            }
            log4net.Config.BasicConfigurator.Configure(m_Appender);
            m_Log = log4net.LogManager.GetLogger("DebugLog");
            //foreach (log4net.ILog log in log4net.LogManager.GetCurrentLoggers())
            //{
            //    log4net.Repository.Hierarchy.Logger logger = (log4net.Repository.Hierarchy.Logger)log.Logger;
            //    Debug.WriteLine(String.Format("{0} Parent {1} Level {2} EffectiveLevel {3}<br>", logger.Name, logger.Parent.Name, logger.Level == null ? "<null>" : logger.Level.Name, logger.EffectiveLevel));
            //}
        }

        /// <summary>
        /// Level allows for different levels of granularity in the log.
        /// Higher levels represent more detail.
        /// </summary>
        public static LogLevel Level
        {
            get { return m_LogLevel; }
            set
            {
                m_LogLevel = value;
                string val = Convert.ToString(value);
                log4net.Repository.Hierarchy.Logger logger = (log4net.Repository.Hierarchy.Logger)m_Log.Logger;

                switch (val)
                {
                    case "Debug":
                        logger.Level = log4net.Core.Level.Debug;
                        m_Appender.Threshold = log4net.Core.Level.Debug;
                        break;
                    case "Info":
                        logger.Level = log4net.Core.Level.Info;
                        m_Appender.Threshold = log4net.Core.Level.Info;
                        break;
                    case "Warning":
                        logger.Level = log4net.Core.Level.Warn;
                        m_Appender.Threshold = log4net.Core.Level.Warn;
                        break;
                    case "Error":
                        logger.Level = log4net.Core.Level.Error;
                        m_Appender.Threshold = log4net.Core.Level.Error;
                        break;
                    case "FatalError":
                        logger.Level = log4net.Core.Level.Severe;
                        m_Appender.Threshold = log4net.Core.Level.Severe;
                        break;
                    default:
                        logger.Level = log4net.Core.Level.Off;
                        m_Appender.Threshold = log4net.Core.Level.Off;
                        break;
                }
                m_Appender.ActivateOptions();
            }
        }

        /// <summary>
        /// Maximum file size.  Sets the size after which file wraps.
        /// </summary>
        public static long MaxFileSize
        {
            get { return m_MaxFileSize; }
            set
            {
                m_MaxFileSize = value;
                m_Appender.MaxFileSize = m_MaxFileSize;
                m_Appender.ActivateOptions();
            }
        }

        /// <summary>
        /// Enable logging.  True means log.  False means don't log.
        /// </summary>
        public static bool Enabled
        {
            get { return m_Enabled; }
            set { m_Enabled = value; }
        }

        /// <summary>
        /// Path represents the physical path name of the file to write to.
        /// </summary>
        public static string Path
        {
            get { return m_Path; }
            set
            {
                m_Path = value;
                m_Appender.File = m_Path;
                m_Appender.ActivateOptions();
            }
        }

        /// <summary>
        /// Write text to the log file.
        /// </summary>
        /// <param name="data">The text to write to the file</param>
        public static void Write(string data)
        {
            Write(data, LogLevel.Debug);
        }

        /// <summary>
        /// Optionally write text to the log file.
        /// </summary>
        /// <param name="data">The text to write to the file</param>
        /// <param name="level">The granularity of the data to write.  
        /// If property level is not set as high as this parameter then don't write to the log file.</param>
        public static void Write(string data, LogLevel level)
        {
            // If logging is disabled then don't write.
            if (!m_Enabled)
                return;

            //  If level is not currently enabled then don't write.
            switch (level)
            {
                case LogLevel.Debug:
                    m_Log.Debug(data);
                    break;

                case LogLevel.Info:
                    m_Log.Info(data);
                    break;

                case LogLevel.Warning:
                    m_Log.Warn(data);
                    break;

                case LogLevel.Error:
                    m_Log.Error(data);
                    break;

                case LogLevel.FatalError:
                    m_Log.Fatal(data);
                    break;
            }
        }
    }

