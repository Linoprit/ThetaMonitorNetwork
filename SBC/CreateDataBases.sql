SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";

--
-- Database: `ThetaMonitor`
--

-- --------------------------------------------------------

--
-- Table structure for table `ID_Table`
--

CREATE TABLE `ID_Table` (
  `PrimKey` int NOT NULL,
  `AddressHash` bigint NOT NULL,
  `AddressIdText` varchar(23) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `SensorType` int NOT NULL,
  `LimitMin` float NOT NULL,
  `LimitMax` float NOT NULL,
  `RelayNr` int DEFAULT NULL,
  `ShortName` varchar(8) NOT NULL,
  `LongName` varchar(64) NOT NULL,
  `Description` varchar(1024) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- --------------------------------------------------------

--
-- Table structure for table `Sensordata`
--

CREATE TABLE `Sensordata` (
  `PrimKey` int NOT NULL,
  `AddressHash` bigint NOT NULL,
  `SensorType` int NOT NULL,
  `Measurement` float NOT NULL,
  `TimeStamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- --------------------------------------------------------

--
-- Table structure for table `Sensortypes`
--

CREATE TABLE `Sensortypes` (
  `PrimKey` int NOT NULL,
  `SensorType` int NOT NULL,
  `Shortname` varchar(12) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `Description` varchar(512) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `Sensortypes`
--

INSERT INTO `Sensortypes` (`PrimKey`, `SensorType`, `Shortname`, `Description`) VALUES
(1, 0, 'Temperature', 'Temperature sensor of any kind'),
(2, 1, 'Humidity', 'Humidity sensor Bosch BME280'),
(3, 2, 'Pressure', 'Barometric pressure sensor Bosch BME280'),
(4, 3, 'Relay', 'Relaystate from station as sensor');

-- --------------------------------------------------------

--
-- Table structure for table `Stationdata`
--

CREATE TABLE `Stationdata` (
  `PrimKey` int NOT NULL,
  `Station_ID` int NOT NULL,
  `Lost_Pkgs` int NOT NULL,
  `Valid_Sensors` int NOT NULL,
  `rxBuff_Overflows` int NOT NULL,
  `Last_Update_Tick` int NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- --------------------------------------------------------

--
-- Table structure for table `StationIds`
--

CREATE TABLE `StationIds` (
  `PrimKey` int NOT NULL,
  `StationId` int NOT NULL,
  `Shortname` varchar(32) NOT NULL,
  `Description` varchar(256) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Indexes for dumped tables
--

--
-- Indexes for table `ID_Table`
--
ALTER TABLE `ID_Table`
  ADD PRIMARY KEY (`PrimKey`);

--
-- Indexes for table `Sensordata`
--
ALTER TABLE `Sensordata`
  ADD PRIMARY KEY (`PrimKey`);

--
-- Indexes for table `Sensortypes`
--
ALTER TABLE `Sensortypes`
  ADD PRIMARY KEY (`PrimKey`);

--
-- Indexes for table `Stationdata`
--
ALTER TABLE `Stationdata`
  ADD PRIMARY KEY (`PrimKey`);

--
-- Indexes for table `StationIds`
--
ALTER TABLE `StationIds`
  ADD PRIMARY KEY (`PrimKey`);

--
-- AUTO_INCREMENT for table `ID_Table`
--
ALTER TABLE `ID_Table`
  MODIFY `PrimKey` int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=501;

--
-- AUTO_INCREMENT for table `Sensordata`
--
ALTER TABLE `Sensordata`
  MODIFY `PrimKey` int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=28;

--
-- AUTO_INCREMENT for table `Sensortypes`
--
ALTER TABLE `Sensortypes`
  MODIFY `PrimKey` int NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `Stationdata`
--
ALTER TABLE `Stationdata`
  MODIFY `PrimKey` int NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `StationIds`
--
ALTER TABLE `StationIds`
  MODIFY `PrimKey` int NOT NULL AUTO_INCREMENT;

COMMIT;

-- --------------------------------------------------------
-- --------------------------------------------------------

--
-- Database: `ThetaMonTest`
--

-- --------------------------------------------------------

--
-- Table structure for table `ID_Table`
--

CREATE TABLE `ID_Table` (
  `PrimKey` int NOT NULL,
  `AddressHash` bigint NOT NULL,
  `AddressIdText` varchar(23) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `SensorType` int NOT NULL,
  `LimitMin` float NOT NULL,
  `LimitMax` float NOT NULL,
  `RelayNr` int DEFAULT NULL,
  `ShortName` varchar(8) NOT NULL,
  `LongName` varchar(64) NOT NULL,
  `Description` varchar(1024) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- --------------------------------------------------------

--
-- Table structure for table `Sensordata`
--

CREATE TABLE `Sensordata` (
  `PrimKey` int NOT NULL,
  `AddressHash` bigint NOT NULL,
  `SensorType` int NOT NULL,
  `Measurement` float NOT NULL,
  `TimeStamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Table structure for table `Sensortypes`
--

CREATE TABLE `Sensortypes` (
  `PrimKey` int NOT NULL,
  `SensorType` int NOT NULL,
  `Shortname` varchar(12) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `Description` varchar(512) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `Sensortypes`
--

INSERT INTO `Sensortypes` (`PrimKey`, `SensorType`, `Shortname`, `Description`) VALUES
(1, 0, 'Temperature', 'Temperature sensor of any kind'),
(2, 1, 'Humidity', 'Humidity sensor Bosch BME280'),
(3, 2, 'Pressure', 'Barometric pressure sensor Bosch BME280'),
(4, 3, 'Relay', 'Relaystate from station as sensor');

-- --------------------------------------------------------

--
-- Table structure for table `StationIds`
--

CREATE TABLE `StationIds` (
  `PrimKey` int NOT NULL,
  `StationId` int NOT NULL,
  `Shortname` varchar(32) NOT NULL,
  `Description` varchar(256) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;



--
-- Indexes for dumped tables
--

--
-- Indexes for table `ID_Table`
--
ALTER TABLE `ID_Table`
  ADD PRIMARY KEY (`PrimKey`);

--
-- Indexes for table `Sensordata`
--
ALTER TABLE `Sensordata`
  ADD PRIMARY KEY (`PrimKey`);

--
-- Indexes for table `Sensortypes`
--
ALTER TABLE `Sensortypes`
  ADD PRIMARY KEY (`PrimKey`);

--
-- Indexes for table `Stationdata`
--
ALTER TABLE `Stationdata`
  ADD PRIMARY KEY (`PrimKey`);
  
--
-- Indexes for table `StationIds`
--
ALTER TABLE `StationIds`
  ADD PRIMARY KEY (`PrimKey`);
--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `ID_Table`
--
ALTER TABLE `ID_Table`
  MODIFY `PrimKey` int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=504;

--
-- AUTO_INCREMENT for table `Sensordata`
--
ALTER TABLE `Sensordata`
  MODIFY `PrimKey` int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=6754;

--
-- AUTO_INCREMENT for table `Sensortypes`
--
ALTER TABLE `Sensortypes`
  MODIFY `PrimKey` int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=5;

--
-- AUTO_INCREMENT for table `Stationdata`
--
ALTER TABLE `Stationdata`
  MODIFY `PrimKey` int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=84;

--
-- AUTO_INCREMENT for table `StationIds`
--
ALTER TABLE `StationIds`
  MODIFY `PrimKey` int NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `StationIds`
--
ALTER TABLE `StationIds`
  MODIFY `PrimKey` int NOT NULL AUTO_INCREMENT;

COMMIT;
